package main

import (
	"assfire.org/api/v1/tsp"
	"context"
	"github.com/golang/protobuf/proto"
	"github.com/google/uuid"
	log "github.com/sirupsen/logrus"
)

type WorkerStatusListener struct {
	statusChannel *AmqpChannelController
}

func (listener *WorkerStatusListener) subscribeForTaskUpdates(ctx context.Context, tasks []string) (<-chan *tsp.WorkerTspStatusUpdate, error) {
	ch := make(chan *tsp.WorkerTspStatusUpdate)

	consumerId := uuid.NewString()

	log.Infof("Using consumer id %s", consumerId)
	go func() {
		queue, err := listener.statusChannel.createQueue("", false, true, false, false, nil, false, false, consumerId, "", StatusExchangeName)
		if err != nil {
			log.Errorf("Failed to create queue for updating status: %v", err)
			close(ch)
			return
		}
		wch, err := queue.consumeWithReconnect()
		if err != nil {
			log.Errorf("Can't consume worker status updates: %v", err)
			close(ch)
			return
		}
		for {
			select {
			case <-ctx.Done():
				log.Infof("Consumer id is done: %s", consumerId)
				close(ch)
				err = queue.cancel()
				if err != nil {
					log.Errorf("Failed to cancel queue %s: %v", queue.queue.Name, err)
				}
				return
			case update := <-wch:
				var parsedUpdate tsp.WorkerTspStatusUpdate
				err := proto.Unmarshal(update.Body, &parsedUpdate)
				if err != nil {
					log.Errorf("Failed to process worker status update: %v", err)
				}
				ch <- &parsedUpdate
			}
		}
	}()

	return ch, nil
}
