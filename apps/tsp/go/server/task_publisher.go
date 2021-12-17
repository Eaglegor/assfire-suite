package main

import (
	"assfire.org/api/v1/tsp"
)

type TaskPublisher struct {
	taskStorage *TaskStorage
	taskQueue   *TaskQueue
}

func (publisher *TaskPublisher) publishTask(taskId string, task *tsp.TspTask) error {
	err := publisher.taskStorage.persistTask(taskId, task)
	if err != nil {
		return err
	}
	err = publisher.taskQueue.sendStartTaskSignal(taskId)
	if err != nil {
		return err
	}
	return nil
}
