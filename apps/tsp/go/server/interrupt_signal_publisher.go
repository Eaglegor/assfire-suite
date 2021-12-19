package main

import (
	"assfire.org/api/v1/tsp"
	"fmt"
	"github.com/golang/protobuf/proto"
)

type InterruptSignalPublisher struct {
	writer *AmqpWriter
}

func (publisher *InterruptSignalPublisher) sendSignal(taskId string, signal_type tsp.WorkerControlSignal_Type) error {
	signalObject := tsp.WorkerControlSignal{
		TaskId:     taskId,
		SignalType: signal_type,
	}

	msg, err := proto.Marshal(&signalObject)
	if err != nil {
		return fmt.Errorf("failed to serialize interrupt signal for task %s: %v", taskId, err)
	}

	return publisher.writer.send(msg)
}

func (publisher *InterruptSignalPublisher) sendInterrupt(taskId string) error {
	return publisher.sendSignal(taskId, tsp.WorkerControlSignal_WORKER_CONTROL_SIGNAL_TYPE_INTERRUPT)
}

func (publisher *InterruptSignalPublisher) sendPause(taskId string) error {
	return publisher.sendSignal(taskId, tsp.WorkerControlSignal_WORKER_CONTROL_SIGNAL_TYPE_PAUSE)
}
