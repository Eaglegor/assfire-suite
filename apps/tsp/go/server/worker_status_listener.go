package main

import (
	"assfire.org/api/v1/tsp"
	"context"
	"github.com/golang/protobuf/proto"
	"github.com/google/uuid"
	log "github.com/sirupsen/logrus"
)

type WorkerStatusListener struct {
	amqpConnector *AmqpConnector
}

func (listener *WorkerStatusListener) subscribeForTaskUpdates(ctx context.Context, tasks []string) (<-chan *tsp.WorkerTspStatusUpdate, error) {
	ch := make(chan *tsp.WorkerTspStatusUpdate)

	consumerId := uuid.NewString()

	log.Infof("Using consumer id %s", consumerId)
	go func() {
		reader, err := listener.amqpConnector.createReader(
			consumerId,
			AmqpReaderOptions{
				queueName:    "",
				exchangeName: StatusExchangeName,
				durable:      false,
				autoDelete:   true,
				exclusive:    false,
				noWait:       false,
				args:         nil,
				routingKey:   "",
				consumerId:   consumerId,
				autoAck:      false,
				noLocal:      false,
			})
		defer func(amqpConnector *AmqpConnector, name string) {
			err := amqpConnector.deleteReader(name)
			if err != nil {
				log.Errorf("Failed to delete reader %s", consumerId)
			}
		}(listener.amqpConnector, consumerId)
		if err != nil {
			log.Errorf("Failed to create AMQP reader for updating status: %v", err)
			close(ch)
			return
		}
		wch, err := reader.createChannel(consumerId)
		if err != nil {
			log.Errorf("Failed to consume AMQP worker status updates: %v", err)
			close(ch)
			return
		}
		for {
			select {
			case <-ctx.Done():
				close(ch)
				return
			case update := <-wch:
				if update.Body == nil {
					continue
				}
				err := reader.ack(update.DeliveryTag)
				if err != nil {
					log.Errorf("Failed to ack status update message: %v", err)
				}
				var parsedUpdate tsp.WorkerTspStatusUpdate
				err = proto.Unmarshal(update.Body, &parsedUpdate)
				if err != nil {
					log.Errorf("Failed to process worker status update: %v", err)
				}
				ch <- &parsedUpdate
			}
		}
	}()

	return ch, nil
}
