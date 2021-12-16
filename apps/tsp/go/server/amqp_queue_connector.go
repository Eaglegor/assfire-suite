package main

import "github.com/streadway/amqp"

type AmqpQueueConnector struct {
	connection *amqp.Connection
	channel    *amqp.Channel
	queue      *amqp.Queue
}

func createTmpAmqpQueue(connection *amqp.Connection, channel *amqp.Channel) *AmqpQueueConnector {
	return &AmqpQueueConnector{
		connection: connection,
		channel:    channel,
		queue:      nil,
	}
}
