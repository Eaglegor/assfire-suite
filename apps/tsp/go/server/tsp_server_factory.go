package main

import (
	"fmt"
	"github.com/go-redis/redis/v8"
	log "github.com/sirupsen/logrus"
	"github.com/streadway/amqp"
)

func createServer(
	redisHost string,
	redisPort int,
	redisPassword string,
	redisDatabase int,
	amqpHost string,
	amqpPort int,
	amqpUser string,
	amqpPassword string,
) (*TspServer, error) {

	log.Infof("Creating TSP server...")

	amqpConnection, err := createAmqpConnectionController(amqpHost, amqpPort, amqpUser, amqpPassword)
	if err != nil {
		return nil, fmt.Errorf("failed to create AMQP connection: %v", err)
	}

	taskChannel, err := amqpConnection.createChannel()
	if err != nil {
		return nil, fmt.Errorf("failed to create AMQP task channel: %v", err)
	}

	taskExchange, err := taskChannel.createExchange(
		TaskExchange,
		amqp.ExchangeDirect,
		true,
		false,
		false,
		false,
		nil,
		TaskQueueName,
		true,
		false,
		"text/plain",
	)
	if err != nil {
		return nil, fmt.Errorf("failed to create AMQP tasks exchange: %v", err)
	}

	taskQueue := &TaskQueue{
		exchange: taskExchange,
	}

	interruptChannel, err := amqpConnection.createChannel()
	if err != nil {
		return nil, fmt.Errorf("failed to create AMQP interrupt channel: %v", err)
	}

	interruptExchange, err := interruptChannel.createExchange(
		InterruptExchange,
		amqp.ExchangeFanout,
		false,
		false,
		false,
		false,
		nil,
		"",
		false,
		false,
		"text/plain",
	)
	if err != nil {
		return nil, fmt.Errorf("failed to create AMQP interrupt exchange: %v", err)
	}

	interruptSignalPublisher := &InterruptSignalPublisher{
		exchange: interruptExchange,
	}

	redisClient := redis.NewClient(&redis.Options{
		Addr:     fmt.Sprintf("%s:%d", redisHost, redisPort),
		Password: redisPassword,
		DB:       redisDatabase,
	})
	redisConnector := &RedisConnector{redisClient}
	taskStorage := &TaskStorage{redisConnector}

	statusChannel, err := amqpConnection.createChannel()
	if err != nil {
		return nil, fmt.Errorf("failed to create AMQP status channel: %v", err)
	}

	workerStatusListener := &WorkerStatusListener{statusChannel}

	statusProvider := &StatusProvider{
		taskStorage:          taskStorage,
		workerStatusListener: workerStatusListener,
	}

	statusEnforcerExchange, err := statusChannel.createExchange(
		StatusExchangeName,
		amqp.ExchangeFanout,
		false,
		false,
		false,
		false,
		nil,
		"",
		false,
		false,
		"text/plain",
	)
	if err != nil {
		return nil, fmt.Errorf("failed to create status enforcer exchange: %v", err)
	}

	statusEnforcer := &StatusEnforcer{
		exchange:    statusEnforcerExchange,
		taskStorage: taskStorage,
	}

	taskPublisher := &TaskPublisher{
		taskStorage: taskStorage,
		taskQueue:   taskQueue,
	}

	solutionStorage := &SolutionStorage{redisConnector}

	return &TspServer{
		taskStorage:              taskStorage,
		solutionStorage:          solutionStorage,
		taskPublisher:            taskPublisher,
		taskQueue:                taskQueue,
		statusEnforcer:           statusEnforcer,
		statusProvider:           statusProvider,
		interruptSignalPublisher: interruptSignalPublisher,
	}, nil

}
