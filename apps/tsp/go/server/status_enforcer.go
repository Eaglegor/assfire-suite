package main

import (
	"assfire.org/api/v1/tsp"
	"context"
	"fmt"
	"github.com/golang/protobuf/proto"
)

type StatusEnforcer struct {
	exchange    *AmqpExchangeConnector
	taskStorage *TaskStorage
}

func (enforcer *StatusEnforcer) subscribeForAllUpdates(ctx context.Context) (chan *tsp.WorkerTspStatusUpdate, error) {
	return make(chan *tsp.WorkerTspStatusUpdate), nil
}

func (enforcer *StatusEnforcer) subscribeForTaskUpdates(ctx context.Context, tasks []string) (chan *tsp.WorkerTspStatusUpdate, error) {
	return make(chan *tsp.WorkerTspStatusUpdate), nil
}

func (enforcer *StatusEnforcer) enforceStatus(taskId string, status tsp.WorkerTspStatusUpdate_Type) error {
	err := enforcer.taskStorage.setTaskStatus(taskId, status)
	if err != nil {
		return fmt.Errorf("failed to enforce persistent status for task %s: %v", taskId, err)
	}

	msg, err := proto.Marshal(&tsp.WorkerTspStatusUpdate{TaskId: taskId, Type: status})
	if err != nil {
		return fmt.Errorf("failed to serialize forced status update for %s: %v", taskId, err)
	}
	return enforcer.exchange.sendWithReconnect(msg)
}
