package main

import (
	"assfire.org/api/v1/tsp"
	"context"
	"errors"
	"fmt"
	"github.com/go-redis/redis/v8"
	"google.golang.org/protobuf/proto"
	"time"
)

const (
	TaskStorageExpiryNs          = time.Hour * 24
	TaskStorageReconnectAttempts = 5
	TaskStorageReconnectsTimeout = 5
)

type TaskStorage struct {
	redisConnector *RedisConnector
}

func (storage *TaskStorage) formatTaskKey(taskId string) string {
	return fmt.Sprintf("%s%s%s", RedisPrefix, taskId, TaskSuffix)
}

func (storage *TaskStorage) persistTask(taskId string, task *tsp.TspTask) error {
	serializedTask, err := proto.Marshal(task)
	if err == nil {
		return storage.redisConnector.Set(
			context.Background(),
			storage.formatTaskKey(taskId),
			serializedTask,
			TaskStorageExpiryNs,
			DefaultRedisRetryPolicy)
	} else {
		return err
	}
}

func (storage *TaskStorage) removeTask(taskId string) error {
	return storage.redisConnector.Del(
		context.Background(),
		taskKey(taskId),
		DefaultRedisRetryPolicy,
	)
}

func (storage *TaskStorage) getTaskStatus(taskId string) (*tsp.TspStatusUpdate, error) {
	result, err := storage.redisConnector.Get(
		context.Background(),
		statusKey(taskId),
		DefaultRedisRetryPolicy,
	)

	if err != nil {
		return &tsp.TspStatusUpdate{
			TaskId: taskId,
			Type:   tsp.TspStatusUpdate_TSP_STATUS_UPDATE_TYPE_UNKNOWN,
		}, fmt.Errorf("failed to obtain status for task %s: %v", taskId, err)
	}
	return &tsp.TspStatusUpdate{
		TaskId: taskId,
		Type:   convertStatusUpdateType(tsp.WorkerTspStatusUpdate_Type(tsp.WorkerTspStatusUpdate_Type_value[result])),
	}, nil
}

func (storage *TaskStorage) setTaskStatus(taskId string, status tsp.WorkerTspStatusUpdate_Type) error {
	return storage.redisConnector.Set(context.Background(), statusKey(taskId), status.String(), 0, DefaultRedisRetryPolicy)
}

var (
	TaskNotFound      = errors.New("task not found")
	TaskStatusUnknown = errors.New("can not determine task status")
)

func (storage *TaskStorage) canBePaused(taskId string) (bool, error) {
	status, err := storage.getTaskStatus(taskId)
	if err != nil {
		if err == redis.Nil {
			return false, TaskNotFound
		} else {
			return false, TaskStatusUnknown
		}
	}
	return status.GetType() == tsp.TspStatusUpdate_TSP_STATUS_UPDATE_TYPE_IN_PROGRESS, nil
}

func (storage *TaskStorage) canBeStopped(taskId string) (bool, error) {
	status, err := storage.getTaskStatus(taskId)
	if err != nil {
		if err == redis.Nil {
			return false, TaskNotFound
		} else {
			return false, TaskStatusUnknown
		}
	}
	return status.GetType() != tsp.TspStatusUpdate_TSP_STATUS_UPDATE_TYPE_FINISHED ||
		status.GetType() != tsp.TspStatusUpdate_TSP_STATUS_UPDATE_TYPE_ERROR ||
		status.GetType() != tsp.TspStatusUpdate_TSP_STATUS_UPDATE_TYPE_UNKNOWN ||
		status.GetType() != tsp.TspStatusUpdate_TSP_STATUS_UPDATE_TYPE_INTERRUPTED, nil
}

func (storage *TaskStorage) canBeResumed(taskId string) (bool, error) {
	status, err := storage.getTaskStatus(taskId)
	if err != nil {
		return false, fmt.Errorf("failed to determine if task %s can be resumed: %v", taskId, err)
	}
	return status.GetType() == tsp.TspStatusUpdate_TSP_STATUS_UPDATE_TYPE_PAUSED, nil
}

func (storage *TaskStorage) taskExists(taskId string) (bool, error) {
	result, err := storage.redisConnector.Exists(
		context.Background(),
		storage.formatTaskKey(taskId),
		DefaultRedisRetryPolicy,
	)

	if err != nil {
		return false, fmt.Errorf("failed to find task %s in storage", taskId)
	}
	return result, nil
}

func (storage *TaskStorage) forEachTaskKey(ctx context.Context, process func(string)) error {
	return storage.redisConnector.Scan(
		ctx,
		RedisPrefix+"*"+TaskSuffix,
		func(key string) {
			process(key[len(RedisPrefix) : len(key)-len(TaskSuffix)])
		},
		DefaultRedisRetryPolicy)
}