package main

import (
	"context"
	"github.com/go-redis/redis/v8"
)

type RedisConnector struct {
	client *redis.Client
}

func (connector *RedisConnector) performWithReconnect(action func(*redis.Client) *redis.StatusCmd) error {
	return action(connector.client).Err()
}

func (connector *RedisConnector) getStringWithReconnect(action func(*redis.Client) *redis.StringCmd) (string, error) {
	result := action(connector.client)
	return result.Val(), result.Err()
}

func (connector *RedisConnector) getInt64WithReconnect(action func(*redis.Client) *redis.IntCmd) (int64, error) {
	result := action(connector.client)
	return result.Val(), result.Err()
}

func (connector *RedisConnector) scanAllWithReconnect(pattern string, process func(string)) error {
	iter := connector.client.Scan(context.Background(), 0, pattern, 0).Iterator()
	for iter.Next(context.Background()) {
		process(iter.Val())
	}
	return iter.Err()
}
