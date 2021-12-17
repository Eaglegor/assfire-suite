package main

import (
	"fmt"
	"github.com/streadway/amqp"
)

type AmqpChannelController struct {
	connectionController *AmqpConnectionController
	channel              *amqp.Channel
	exchanges            map[string]*AmqpExchangeConnector
	queues               map[string]*AmqpQueueConnector
}

type AmqpConnectionController struct {
	connection *amqp.Connection
	channels   map[int]*AmqpChannelController
}

func createAmqpConnectionController(
	amqpHost string,
	amqpPort int,
	amqpUser string,
	amqpPassword string,
) (*AmqpConnectionController, error) {

	amqpConnection, err := amqp.Dial(fmt.Sprintf("amqp://%s:%s@%s:%d", amqpUser, amqpPassword, amqpHost, amqpPort))
	if err != nil {
		return nil, err
	}

	return &AmqpConnectionController{
		connection: amqpConnection,
		channels:   make(map[int]*AmqpChannelController),
	}, nil
}

func (connectionController *AmqpConnectionController) createChannel() (*AmqpChannelController, error) {
	channel, err := connectionController.connection.Channel()
	if err != nil {
		return nil, err
	}
	controller := &AmqpChannelController{
		connectionController: connectionController,
		channel:              channel,
		exchanges:            make(map[string]*AmqpExchangeConnector),
		queues:               make(map[string]*AmqpQueueConnector),
	}
	connectionController.channels[len(connectionController.channels)] = controller
	return controller, nil
}

func (channelController *AmqpChannelController) createExchange(
	name string,
	exchangeType string,
	durable bool,
	autoDelete bool,
	internal bool,
	noWait bool,
	args amqp.Table,
	routingKey string,
	mandatory bool,
	immediate bool,
	contentType string,
) (*AmqpExchangeConnector, error) {
	err := channelController.channel.ExchangeDeclare(
		name,
		exchangeType,
		durable,
		autoDelete,
		internal,
		noWait,
		args,
	)
	if err != nil {
		return nil, err
	}
	connector := &AmqpExchangeConnector{
		channelController: channelController,
		exchangeName:      name,
		routingKey:        routingKey,
		mandatory:         mandatory,
		immediate:         immediate,
		contentType:       contentType,
	}
	channelController.exchanges[name] = connector
	return connector, nil
}

func (channelController *AmqpChannelController) createQueue(
	name string,
	durable bool,
	autoDelete bool,
	exclusive bool,
	noWait bool,
	args amqp.Table,
	noLocal bool,
	autoAck bool,
	consumerId string,
) (*AmqpQueueConnector, error) {
	queue, err := channelController.channel.QueueDeclare(
		name,
		durable,
		autoDelete,
		exclusive,
		noWait,
		args,
	)
	if err != nil {
		return nil, err
	}
	connector := &AmqpQueueConnector{
		channelController: channelController,
		queue:             queue,
		consumerId:        consumerId,
		autoAck:           autoAck,
		exclusive:         exclusive,
		noLocal:           noLocal,
		noWait:            noWait,
		args:              args,
	}
	channelController.queues[name] = connector
	return connector, nil
}
