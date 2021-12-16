package main

import (
	"assfire.org/api/v1/tsp"
	"context"
)

type WorkerStatusListener struct {
	queue *AmqpQueueConnector
}

func (listener *WorkerStatusListener) subscribeForAllUpdates(ctx context.Context) (chan *tsp.WorkerTspStatusUpdate, error) {
	return make(chan *tsp.WorkerTspStatusUpdate), nil
}

func (listener *WorkerStatusListener) subscribeForTaskUpdates(ctx context.Context, tasks []string) (chan *tsp.WorkerTspStatusUpdate, error) {
	return make(chan *tsp.WorkerTspStatusUpdate), nil
}
