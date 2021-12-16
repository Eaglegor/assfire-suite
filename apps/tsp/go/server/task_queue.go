package main

import (
	"assfire.org/api/v1/tsp"
	"fmt"
	"github.com/golang/protobuf/proto"
)

type TaskQueue struct {
	exchange *AmqpExchangeConnector
}

func (queue *TaskQueue) sendNewTaskId(taskId string) error {
	msg, err := proto.Marshal(&tsp.WorkerTask{TaskId: taskId})

	if err != nil {
		return fmt.Errorf("failed to serialize new task signal for %s: %v", taskId, err)
	}

	return queue.exchange.sendWithReconnect(msg)
}
