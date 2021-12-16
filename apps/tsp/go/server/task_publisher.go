package main

import (
	"assfire.org/api/v1/tsp"
	"context"
	"github.com/streadway/amqp"
)

type TaskPublisher struct {
	connection     *amqp.Connection
	publishChannel *amqp.Channel
	publishQueue   *amqp.Queue
	taskStorage    *TaskStorage
}

func (publisher *TaskPublisher) publishTask(ctx context.Context, taskId string, task *tsp.TspTask) error {
	return publisher.taskStorage.persistTask(taskId, task)
}
