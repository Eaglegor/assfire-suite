package main

import (
	"assfire.org/api/v1/tsp"
	"context"
	"fmt"
	"log"
)

type StatusProvider struct {
	taskStorage          *TaskStorage
	workerStatusListener *WorkerStatusListener
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

func (provider *StatusProvider) getLatestStatus(taskId string) (*tsp.TspStatusUpdate, error) {
	status, err := provider.taskStorage.getTaskStatus(taskId)
	if err != nil {
		return nil, fmt.Errorf("failed to retrieve latest status for task %s: %v", taskId, err)
	}
	return status, nil
}

func (provider *StatusProvider) createInvalidStatus(taskId string) *tsp.TspStatusUpdate {
	return &tsp.TspStatusUpdate{
		TaskId: taskId,
		Type:   tsp.TspStatusUpdate_TSP_STATUS_UPDATE_TYPE_UNKNOWN,
	}
}

func (provider *StatusProvider) subscribeForStoredTaskStatus(ch chan *tsp.TspStatusUpdate, ctx context.Context, taskIds []string) error {
	if len(taskIds) != 0 {
		for _, taskId := range taskIds {
			capturedId := taskId
			go func() {
				status, err := provider.getLatestStatus(capturedId)
				if err == nil {
					ch <- status
				} else {
					ch <- provider.createInvalidStatus(capturedId)
				}
			}()
		}
	} else {
		go func() {
			err := provider.taskStorage.forEachTaskKey(ctx, func(key string) {
				status, err := provider.getLatestStatus(key)
				if err == nil {
					ch <- status
				} else {
					ch <- provider.createInvalidStatus(key)
				}
			})
			if err != nil {
				log.Printf("Error whil trying to iterate over all task keys: %v", err)
			}
		}()
	}
	return nil
}

func (provider *StatusProvider) subscribeForWorkerStatusUpdates(ctx context.Context, ch chan<- *tsp.TspStatusUpdate, taskIds []string) error {
	wch, err := provider.workerStatusListener.subscribeForTaskUpdates(ctx, taskIds)
	if err != nil {
		return fmt.Errorf("failed to subscribe for worker task updates: %v", err)
	}

	go func() {
		for {
			select {
			case <-ctx.Done():
				close(ch)
				return
			case upd := <-wch:
				ch <- &tsp.TspStatusUpdate{
					TaskId:                      upd.GetTaskId(),
					Type:                        convertStatusUpdateType(upd.GetType()),
					NewSolutionCost:             upd.GetCost(),
					NewSolutionValidationResult: upd.GetValidationResult(),
				}
			}
		}
	}()

	return nil
}

func (provider *StatusProvider) subscribeForStatusUpdates(ctx context.Context, taskIds []string) (<-chan *tsp.TspStatusUpdate, error) {
	taskStatusChannel := make(chan *tsp.TspStatusUpdate)

	err := provider.subscribeForStoredTaskStatus(taskStatusChannel, ctx, taskIds)
	if err != nil {
		log.Printf("Failed to retrieve latest status from storage (task selector: %v: %v", taskIds, err)
	}

	workerUpdateChannel := make(chan *tsp.TspStatusUpdate)
	err = provider.subscribeForWorkerStatusUpdates(ctx, workerUpdateChannel, taskIds)
	if err != nil {
		return nil, fmt.Errorf("failed to subscribe to worker status updates channelController: %v", err)
	}

	ch := make(chan *tsp.TspStatusUpdate)

	go func() {
		for {
			select {
			case <-ctx.Done():
				close(ch)
				return
			case taskUpdate := <-taskStatusChannel:
				ch <- taskUpdate
			case workerUpdate := <-workerUpdateChannel:
				ch <- workerUpdate
			}
		}
	}()

	return ch, nil
}
