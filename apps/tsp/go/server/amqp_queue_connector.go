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
}

func (connector *AmqpQueueConnector) consumeWithReconnect() (<-chan amqp.Delivery, error) {
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
