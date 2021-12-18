package main

import (
	"fmt"
	log "github.com/sirupsen/logrus"
	"github.com/streadway/amqp"
	"sync"
)

type AmqpConnectorOptions struct {
	amqpHost     string
	amqpPort     int
	amqpUser     string
	amqpPassword string
}

type AmqpConnector struct {
	mutex      sync.Mutex
	options    AmqpConnectorOptions
	readers    map[string]*AmqpReader
	writers    map[string]*AmqpWriter
	connection *amqp.Connection
}

type AmqpWriterOptions struct {
	exchangeName string
	exchangeType string
	durable      bool
	autoDelete   bool
	internal     bool
	noWait       bool
	args         amqp.Table
	routingKey   string
	mandatory    bool
	immediate    bool
	contentType  string
}

type AmqpWriter struct {
	name      string
	connector *AmqpConnector
	options   AmqpWriterOptions
	channel   *amqp.Channel
}

type AmqpReaderOptions struct {
	queueName    string
	exchangeName string
	durable      bool
	autoDelete   bool
	exclusive    bool
	noWait       bool
	args         amqp.Table
	routingKey   string
	consumerId   string
	autoAck      bool
	noLocal      bool
}

type AmqpReader struct {
	mutex        sync.RWMutex
	name         string
	connector    *AmqpConnector
	options      AmqpReaderOptions
	upChannel    *amqp.Channel
	downChannels map[string]chan amqp.Delivery
	cancel       chan *amqp.Channel
}

func createConnection(options AmqpConnectorOptions) (*amqp.Connection, error) {
	return amqp.Dial(fmt.Sprintf("amqp://%s:%s@%s:%d", options.amqpUser, options.amqpPassword, options.amqpHost, options.amqpPort))
}

func createAmqpConnector(options AmqpConnectorOptions) (*AmqpConnector, error) {
	connector := &AmqpConnector{
		mutex:   sync.Mutex{},
		options: options,
		readers: make(map[string]*AmqpReader),
		writers: make(map[string]*AmqpWriter),
	}

	conn, err := createConnection(options)
	if err != nil {
		return nil, err
	}

	err = connector.replaceConnection(conn)
	if err != nil {
		return nil, err
	}

	return connector, nil
}

func (connector *AmqpConnector) replaceConnection(connection *amqp.Connection) error {
	connector.mutex.Lock()
	defer connector.mutex.Unlock()
	connector.connection = connection

	for _, reader := range connector.readers {
		ch, err := connector.connection.Channel()
		if err != nil {
			return err
		}
		err = reader.replaceChannel(ch)
		if err != nil {
			return err
		}
	}

	for _, writer := range connector.writers {
		ch, err := connector.connection.Channel()
		if err != nil {
			return err
		}
		err = writer.replaceChannel(ch)
		if err != nil {
			return err
		}
	}

	return nil
}

func (connector *AmqpConnector) createWriter(name string, options AmqpWriterOptions) (*AmqpWriter, error) {
	connector.mutex.Lock()
	defer connector.mutex.Unlock()
	if connector.writers[name] != nil {
		return nil, fmt.Errorf("writer %s already exists", name)
	}

	connector.writers[name] = &AmqpWriter{
		name:      name,
		connector: connector,
		options:   options,
	}
	return connector.writers[name], nil
}

func (connector *AmqpConnector) createReader(name string, options AmqpReaderOptions) (*AmqpReader, error) {
	connector.mutex.Lock()
	defer connector.mutex.Unlock()
	if connector.readers[name] != nil {
		return nil, fmt.Errorf("reader %s already exists", name)
	}
	connector.readers[name] = &AmqpReader{
		mutex:        sync.RWMutex{},
		name:         name,
		connector:    connector,
		options:      options,
		downChannels: make(map[string]chan amqp.Delivery),
		cancel:       make(chan *amqp.Channel),
	}
	return connector.readers[name], nil
}

func (connector *AmqpConnector) deleteWriter(name string) error {
	connector.mutex.Lock()
	defer connector.mutex.Unlock()
	err := connector.writers[name].close()
	if err != nil {
		log.Errorf("Failed to close writer %s", name)
	}
	delete(connector.writers, name)
	return nil
}

func (connector *AmqpConnector) deleteReader(name string) error {
	connector.mutex.Lock()
	defer connector.mutex.Unlock()
	err := connector.readers[name].close()
	if err != nil {
		log.Errorf("Failed to close reader %s", name)
	}
	delete(connector.readers, name)
	return nil
}

func (reader *AmqpReader) createChannel(name string) <-chan amqp.Delivery {
	reader.mutex.Lock()
	defer reader.mutex.Unlock()

	reader.downChannels[name] = make(chan amqp.Delivery)
	return reader.downChannels[name]
}

func (reader *AmqpReader) close() error {
	return nil
}

func (reader *AmqpReader) replaceChannel(channel *amqp.Channel) error {
	if reader.upChannel != nil {
		reader.cancel <- reader.upChannel
	}
	reader.upChannel = channel
	return reader.bind()
}

func (reader *AmqpReader) bind() error {
	_, err := reader.upChannel.QueueDeclare(
		reader.options.queueName,
		reader.options.durable,
		reader.options.autoDelete,
		reader.options.exclusive,
		reader.options.noWait,
		reader.options.args,
	)
	if err != nil {
		return err
	}

	err = reader.upChannel.QueueBind(
		reader.options.queueName,
		reader.options.routingKey,
		reader.options.exchangeName,
		reader.options.noWait,
		reader.options.args,
	)

	if err != nil {
		return err
	}

	ch, err := reader.upChannel.Consume(
		reader.options.queueName,
		reader.options.consumerId,
		reader.options.autoAck,
		reader.options.exclusive,
		reader.options.noLocal,
		reader.options.noWait,
		reader.options.args,
	)

	if err != nil {
		unbindError := reader.upChannel.QueueUnbind(
			reader.options.queueName,
			reader.options.routingKey,
			reader.options.exchangeName,
			reader.options.args,
		)
		if unbindError != nil {
			log.Errorf("Failed to unbind queue on error: %v, unbind caused by %v", unbindError, err)
		}
		return err
	}

	go func() {
		currentChan := reader.upChannel
		for {
			select {
			case signal := <-reader.cancel:
				if signal == currentChan {
					log.Infof("Unsubscribing from old AMQP channel for reader %s", reader.name)
					return
				}
			case update := <-ch:
				reader.mutex.RLock()
				for _, downChannel := range reader.downChannels {
					downChannel <- update
				}
				reader.mutex.RUnlock()
			}
		}
	}()

	return nil
}

func (writer *AmqpWriter) replaceChannel(channel *amqp.Channel) error {
	writer.channel = channel
	return writer.bind()
}

func (writer *AmqpWriter) bind() error {
	err := writer.channel.ExchangeDeclare(
		writer.options.exchangeName,
		writer.options.exchangeType,
		writer.options.durable,
		writer.options.autoDelete,
		writer.options.internal,
		writer.options.noWait,
		writer.options.args,
	)
	if err != nil {
		return err
	}

	return err
}

func (writer *AmqpWriter) write(msg []byte) error {
	return writer.channel.Publish(
		writer.options.exchangeName,
		writer.options.routingKey,
		writer.options.mandatory,
		writer.options.immediate,
		amqp.Publishing{
			ContentType: writer.options.contentType,
			Body:        msg,
		},
	)
}

func (writer *AmqpWriter) close() error {
	return nil
}
