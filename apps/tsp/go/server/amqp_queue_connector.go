package main

import (
	"fmt"
	"github.com/streadway/amqp"
)

type AmqpQueueConnector struct {
	channelController *AmqpChannelController
	queue             amqp.Queue
	consumerId        string
	autoAck           bool
	exclusive         bool
	noLocal           bool
	noWait            bool
	args              amqp.Table
	routingKey        string
	exchangeName      string
}

func (connector *AmqpQueueConnector) consumeWithReconnect() (<-chan amqp.Delivery, error) {
	err := connector.channelController.channel.QueueBind(
		connector.queue.Name,
		connector.routingKey,
		connector.exchangeName,
		connector.noWait,
		connector.args,
	)

	if err != nil {
		return nil, fmt.Errorf("failed to bind queue %s: %v", connector.queue.Name, err)
	}
	ch, err := connector.channelController.channel.Consume(
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

func (connector *AmqpQueueConnector) cancel() error {
	return connector.channelController.channel.Cancel(connector.consumerId, false)
}
