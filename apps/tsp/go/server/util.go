package main

import (
	"assfire.org/api/v1/tsp"
	"fmt"
)

const (
	PauseSignal     = iota
	InterruptSignal = iota

	RedisPrefix    = "assfire.tsp."
	TaskSuffix     = ".task"
	StatusSuffix   = ".status"
	SolutionSuffix = ".solution"

	TaskExchange       = "amq.direct"
	InterruptExchange  = "assfire.tsp.worker.interrupt"
	StatusExchangeName = "assfire.tsp.worker.status"

	TaskQueueName = "assfire.tsp.worker.task"
)

func taskKey(taskId string) string {
	return fmt.Sprintf("%s%s%s", RedisPrefix, taskId, TaskSuffix)
}

func statusKey(taskId string) string {
	return fmt.Sprintf("%s%s%s", RedisPrefix, taskId, StatusSuffix)
}

func solutionKey(taskId string) string {
	return fmt.Sprintf("%s%s%s", RedisPrefix, taskId, SolutionSuffix)
}

func convertStatusUpdateType(workerType tsp.WorkerTspStatusUpdate_Type) tsp.TspStatusUpdate_Type {
	switch workerType {
	case tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_PENDING:
		return tsp.TspStatusUpdate_TSP_STATUS_UPDATE_TYPE_PENDING
	case tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_NEW_SOLUTION:
		return tsp.TspStatusUpdate_TSP_STATUS_UPDATE_TYPE_NEW_SOLUTION
	case tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_ERROR:
		return tsp.TspStatusUpdate_TSP_STATUS_UPDATE_TYPE_ERROR
	case tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_FINISHED:
		return tsp.TspStatusUpdate_TSP_STATUS_UPDATE_TYPE_FINISHED
	case tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_PAUSED:
		return tsp.TspStatusUpdate_TSP_STATUS_UPDATE_TYPE_PAUSED
	case tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_INTERRUPTED:
		return tsp.TspStatusUpdate_TSP_STATUS_UPDATE_TYPE_INTERRUPTED
	case tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_STARTED:
		return tsp.TspStatusUpdate_TSP_STATUS_UPDATE_TYPE_STARTED
	case tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_IN_PROGRESS:
		return tsp.TspStatusUpdate_TSP_STATUS_UPDATE_TYPE_IN_PROGRESS
	default:
		return tsp.TspStatusUpdate_TSP_STATUS_UPDATE_TYPE_UNKNOWN
	}
}
