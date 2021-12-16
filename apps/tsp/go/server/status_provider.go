package main

import (
	"assfire.org/api/v1/tsp"
	"fmt"
)

type StatusProvider struct {
	taskStorage *TaskStorage
}

func (provider *StatusProvider) formatStatusKey(taskId string) string {
	return fmt.Sprintf("%s%s%s", RedisPrefix, taskId, StatusSuffix)
}

func (provider *StatusProvider) isTerminalState(state tsp.WorkerTspStatusUpdate_Type) bool {
	return state == tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_ERROR ||
		state == tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_FINISHED ||
		state == tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_INTERRUPTED ||
		state == tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_UNKNOWN
}

func (provider *StatusProvider) getLatestStatus(taskId string) (*TspStatusUpdate, error) {
	status, err := provider.taskStorage.getTaskStatus(taskId)
	if err != nil {
		return nil, fmt.Errorf("failed to retrieve latest status for task %s: %v", taskId, err)
	}
	return &TspStatusUpdate{
		taskId: taskId,
		status: status,
	}, nil
}
