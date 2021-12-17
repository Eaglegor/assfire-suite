package main

import (
	"assfire.org/api/v1/tsp"
	"context"
	"errors"
	"fmt"
	"github.com/go-redis/redis/v8"
	"github.com/golang/protobuf/proto"
)

type SolutionStorage struct {
	redisConnector *RedisConnector
}

func (storage *SolutionStorage) formatSolutionKey(taskId string) string {
	return fmt.Sprintf("%s%s%s", RedisPrefix, taskId, SolutionSuffix)
}

var (
	SolutionNotFound = errors.New("solution not found")
)

func (storage *SolutionStorage) loadSolution(taskId string) (*tsp.TspSolution, error) {
	result, err := storage.redisConnector.getStringWithReconnect(func(client *redis.Client) *redis.StringCmd {
		return client.Get(context.Background(), storage.formatSolutionKey(taskId))
	})
	if err != nil {
		if err == redis.Nil {
			return nil, SolutionNotFound
		} else {
			return nil, fmt.Errorf("failed to load solution for %s: %v", taskId, err)
		}
	}

	var solution tsp.TspSolution

	err = proto.Unmarshal([]byte(result), &solution)
	if err != nil {
		return nil, fmt.Errorf("failed to parse solution for %s: %v", taskId, err)
	}

	return &solution, nil
}
