package main

import (
	"fmt"
	"github.com/streadway/amqp"
)

type AmqpQueueConnector struct {
	connection *amqp.Connection
	channel    *amqp.Channel
	queue      *amqp.Queue
	consumerId string
	autoAck    bool
	exclusive  bool
	noLocal    bool
	noWait     bool
	args       amqp.Table
}

func createTmpAmqpQueue(connection *amqp.Connection, channel *amqp.Channel) *AmqpQueueConnector {
	return &AmqpQueueConnector{
		connection: connection,
		channel:    channel,
		queue:      nil,
	}
}

func (connector *AmqpQueueConnector) consumeWithReconnect() (<-chan amqp.Delivery, error) {
	ch, err := connector.channel.Consume(
		connector.queue.Name,
		connector.consumerId,
		connector.autoAck,
		connector.exclusive,
		connector.noLocal,
		connector.noWait,
		connector.args,
	)
	if err != nil {
		return nil, fmt.Errorf("failed to consume messages from queue %s (consumer id %s): %v", connector.queue.Name, connector.consumerId, err)
	}
	return ch, nil
}
