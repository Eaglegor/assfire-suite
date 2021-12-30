package main

import (
	"context"
	"github.com/go-redis/redis/v8"
	log "github.com/sirupsen/logrus"
	"time"
)

type RedisConnector struct {
	client *redis.Client
}

type RedisRetryPolicy struct {
	attempts int
	interval time.Duration
}

var (
	DefaultRedisRetryPolicy = RedisRetryPolicy{
		attempts: 5,
		interval: 5 * time.Second,
	}
)

func (connector *RedisConnector) Set(
	ctx context.Context,
	key string,
	value interface{},
	expiration time.Duration,
	retryPolicy RedisRetryPolicy) error {
	_, err := doWithReconnect(connector.client, func(client *redis.Client) (interface{}, error) {
		return nil, client.Set(ctx, key, value, expiration).Err()
	}, retryPolicy)
	return err
}

func (connector *RedisConnector) Del(
	ctx context.Context,
	key string,
	retryPolicy RedisRetryPolicy) error {
	_, err := doWithReconnect(connector.client, func(client *redis.Client) (interface{}, error) {
		return nil, client.Del(ctx, key).Err()
	}, retryPolicy)
	return err
}

func (connector *RedisConnector) Get(
	ctx context.Context,
	key string,
	retryPolicy RedisRetryPolicy,
) (string, error) {
	val, err := doWithReconnect(connector.client, func(client *redis.Client) (interface{}, error) {
		cmd := client.Get(ctx, key)
		return cmd.Val(), cmd.Err()
	}, retryPolicy)
	return val.(string), err
}

func (connector *RedisConnector) GetAndParse(
	ctx context.Context,
	key string,
	parse func(string) (interface{}, error),
	retryPolicy RedisRetryPolicy,
) (interface{}, error) {
	val, err := connector.Get(ctx, key, retryPolicy)
	if err != nil {
		return nil, err
	} else {
		return parse(val)
	}
}

func (connector *RedisConnector) Exists(
	ctx context.Context,
	key string,
	retryPolicy RedisRetryPolicy) (bool, error) {
	val, err := doWithReconnect(connector.client, func(client *redis.Client) (interface{}, error) {
		cmd := client.Exists(ctx, key)
		return cmd.Val() != 0, cmd.Err()
	}, retryPolicy)
	return val.(bool), err
}

func (connector *RedisConnector) Scan(
	ctx context.Context,
	pattern string,
	process func(string),
	retryPolicy RedisRetryPolicy) error {

	val, err := doWithReconnect(connector.client, func(client *redis.Client) (interface{}, error) {
		cmd := connector.client.Scan(ctx, 0, pattern, 0)
		return cmd.Iterator(), cmd.Err()
	}, retryPolicy)

	if err != nil {
		return err
	}

	iter := val.(*redis.ScanIterator)
	for iter.Next(ctx) {
		process(iter.Val())
	}
	return iter.Err()
}

func doWithReconnect(client *redis.Client, action func(*redis.Client) (interface{}, error), retryPolicy RedisRetryPolicy) (interface{}, error) {
	for i := 0; ; i++ {
		val, err := action(client)
		if err == nil || err == redis.Nil || i >= retryPolicy.attempts-1 {
			return val, err
		}
		log.Errorf("Error while making call to redis server: %v. Retrying in %v", err, retryPolicy.interval)
		time.Sleep(retryPolicy.interval)
	}
}
