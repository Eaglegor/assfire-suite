package main

import (
	"assfire.org/api/v1/tsp"
	"context"
)

type WorkerStatusListener struct {
	queue *AmqpQueueConnector
}

func (listener *WorkerStatusListener) subscribeForTaskUpdates(ctx context.Context, tasks []string) (<-chan *tsp.WorkerTspStatusUpdate, error) {
	ch := make(chan *tsp.WorkerTspStatusUpdate)

	return ch, nil
}
