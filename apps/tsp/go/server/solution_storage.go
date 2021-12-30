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
	result, err := storage.redisConnector.GetAndParse(
		context.Background(),
		storage.formatSolutionKey(taskId),
		func(val string) (interface{}, error) {
			solution := &tsp.TspSolution{}
			err := proto.Unmarshal([]byte(val), solution)
			return solution, err
		},
		DefaultRedisRetryPolicy,
	)

	if err != nil {
		if err == redis.Nil {
			return nil, SolutionNotFound
		} else {
			return nil, fmt.Errorf("failed to load solution for %s: %v", taskId, err)
		}
	}

	return result.(*tsp.TspSolution), nil
}
