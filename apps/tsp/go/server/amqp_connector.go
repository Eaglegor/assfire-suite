package main

import (
	"fmt"
	log "github.com/sirupsen/logrus"
	"github.com/streadway/amqp"
	"sync"
	"time"
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

func (connector *AmqpConnector) createChannel() (*amqp.Channel, error) {
	return connector.connection.Channel()
}

func (connector *AmqpConnector) startReconnectLoop(connection *amqp.Connection, interval time.Duration) {
	log.Infof("Scheduling AMQP reconnect loop with interval %v", interval)
	ch := connection.NotifyClose(make(chan *amqp.Error))
	for signal := range ch {
		log.Errorf("AMQP connection broken: %v. Trying to reconnect", signal)

		done := false

		for !done {
			conn, err := createConnection(connector.options)
			if err != nil {
				log.Errorf("Failed to reconnect AMQP connector: %v. Will retry in %v", err, interval)
				time.Sleep(interval)
				continue
			}
			err = connector.replaceConnection(conn)
			if err != nil {
				log.Errorf("Failed to replace connection in AMQP connector: %v. Will retry in %v", err, interval)
				time.Sleep(interval)
				continue
			}
			log.Infof("AMQP connection re-established")
			done = true
			go connector.startReconnectLoop(conn, interval)
		}
	}
}

func createAmqpConnector(options AmqpConnectorOptions) (*AmqpConnector, error) {
	log.Infof("Creating AMQP connector to %s:%d", options.amqpHost, options.amqpPort)
	connector := &AmqpConnector{
		mutex:   sync.Mutex{},
		options: options,
		readers: make(map[string]*AmqpReader),
		writers: make(map[string]*AmqpWriter),
	}

	var connection *amqp.Connection
	for {
		conn, err := createConnection(options)
		if err != nil {
			log.Infof("Failed to connect to %s:%d: %v. Will retry in 5s", options.amqpHost, options.amqpPort, err)
			time.Sleep(5 * time.Second)
			continue
		}
		connection = conn
		break
	}

	err := connector.replaceConnection(connection)
	if err != nil {
		return nil, err
	}

	go connector.startReconnectLoop(connection, time.Second*5)

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
	log.Infof("Creating AMQP writer %s", name)
	connector.mutex.Lock()
	defer connector.mutex.Unlock()
	if connector.writers[name] != nil {
		return nil, fmt.Errorf("writer %s already exists", name)
	}

	ch, err := connector.createChannel()
	if err != nil {
		return nil, fmt.Errorf("failed to create AMQP channel for writer: %v", err)
	}

	writer := &AmqpWriter{
		name:      name,
		connector: connector,
		options:   options,
	}
	err = writer.replaceChannel(ch)
	if err != nil {
		return nil, fmt.Errorf("failed to set new AMQP channel for writer: %v", err)
	}

	connector.writers[name] = writer
	return writer, nil
}

func (connector *AmqpConnector) createReader(name string, options AmqpReaderOptions) (*AmqpReader, error) {
	log.Infof("Creating AMQP reader %s", name)
	connector.mutex.Lock()
	defer connector.mutex.Unlock()
	if connector.readers[name] != nil {
		return nil, fmt.Errorf("reader %s already exists", name)
	}

	ch, err := connector.createChannel()
	if err != nil {
		return nil, fmt.Errorf("failed to create AMQP channel for reader: %v", err)
	}

	reader := &AmqpReader{
		mutex:        sync.RWMutex{},
		name:         name,
		connector:    connector,
		options:      options,
		downChannels: make(map[string]chan amqp.Delivery),
		cancel:       make(chan *amqp.Channel),
	}

	err = reader.replaceChannel(ch)
	if err != nil {
		return nil, fmt.Errorf("failed to set new AMQP channel for reader: %v", err)
	}

	connector.readers[name] = reader

	return reader, nil
}

func (connector *AmqpConnector) deleteWriter(name string) error {
	log.Infof("Destroying AMQP writer %s", name)
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
	log.Infof("Destroying AMQP reader %s", name)
	connector.mutex.Lock()
	defer connector.mutex.Unlock()
	err := connector.readers[name].close()
	if err != nil {
		log.Errorf("Failed to close reader %s", name)
	}
	delete(connector.readers, name)
	return nil
}

func (reader *AmqpReader) createChannel(name string) (<-chan amqp.Delivery, error) {
	log.Infof("Creating AMQP reader channel %s/%s", reader.name, name)
	reader.mutex.Lock()
	defer reader.mutex.Unlock()

	reader.downChannels[name] = make(chan amqp.Delivery)
	return reader.downChannels[name], nil
}

func (reader *AmqpReader) ack(tag uint64) error {
	return reader.upChannel.Ack(tag, false)
}

func (reader *AmqpReader) close() error {
	err := reader.upChannel.Close()
	return err
}

func (reader *AmqpReader) replaceChannel(channel *amqp.Channel) error {
	if reader.upChannel != nil {
		reader.cancel <- reader.upChannel
	}
	reader.upChannel = channel
	return reader.bind()
}

func (reader *AmqpReader) bind() error {
	log.Infof("Binding queue for reader %s", reader.name)
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

	closeChannel := reader.upChannel.NotifyClose(make(chan *amqp.Error))
	go func() {
		for signal := range closeChannel {
			log.Errorf("AMQP channel for %s broken: %v", reader.name, signal)
			reader.cancel <- reader.upChannel
			reader.upChannel = nil
			<-reader.cancel
		}
	}()

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
	log.Infof("Binding exchange for writer %s", writer.name)
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

func (writer *AmqpWriter) send(msg []byte) error {
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
	return writer.channel.Close()
}
