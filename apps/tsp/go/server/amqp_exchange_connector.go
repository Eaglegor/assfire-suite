package main

import "github.com/streadway/amqp"

type AmqpExchangeConnector struct {
	connection   *amqp.Connection
	channel      *amqp.Channel
	exchangeName string
	routingKey   string
	mandatory    bool
	immediate    bool
	contentType  string
}

func (connector *AmqpExchangeConnector) sendWithReconnect(msg []byte) error {
	return connector.channel.Publish(
		connector.exchangeName,
		connector.routingKey,
		connector.mandatory,
		connector.immediate,
		amqp.Publishing{
			ContentType: connector.contentType,
			Body:        msg,
		},
	)
}
