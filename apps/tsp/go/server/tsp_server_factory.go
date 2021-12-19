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

	amqpConnector, err := createAmqpConnector(AmqpConnectorOptions{
		amqpHost:     amqpHost,
		amqpPort:     amqpPort,
		amqpUser:     amqpUser,
		amqpPassword: amqpPassword,
	})
	if err != nil {
		return nil, fmt.Errorf("failed to create AMQP connector: %v", err)
	}

	taskWriter, err := amqpConnector.createWriter("TaskWriter",
		AmqpWriterOptions{
			exchangeName: TaskExchange,
			exchangeType: amqp.ExchangeDirect,
			durable:      true,
			autoDelete:   false,
			internal:     false,
			noWait:       false,
			args:         nil,
			routingKey:   TaskQueueName,
			mandatory:    true,
			immediate:    false,
			contentType:  "text/plain",
		},
	)

	if err != nil {
		return nil, fmt.Errorf("failed to create AMQP writer for publishing tasks: %v", err)
	}

	taskQueue := &TaskQueue{
		writer: taskWriter,
	}

	interruptWriter, err := amqpConnector.createWriter("InterruptWriter",
		AmqpWriterOptions{
			exchangeName: InterruptExchange,
			exchangeType: amqp.ExchangeFanout,
			durable:      true,
			autoDelete:   false,
			internal:     false,
			noWait:       false,
			args:         nil,
			routingKey:   "",
			mandatory:    false,
			immediate:    false,
			contentType:  "text/plain",
		},
	)

	if err != nil {
		return nil, fmt.Errorf("failed to create interrupt signal AMQP writer: %v", err)
	}

	interruptSignalPublisher := &InterruptSignalPublisher{
		writer: interruptWriter,
	}

	redisClient := redis.NewClient(&redis.Options{
		Addr:     fmt.Sprintf("%s:%d", redisHost, redisPort),
		Password: redisPassword,
		DB:       redisDatabase,
	})
	redisConnector := &RedisConnector{redisClient}
	taskStorage := &TaskStorage{redisConnector}

	workerStatusListener := &WorkerStatusListener{
		amqpConnector: amqpConnector,
	}

	statusProvider := &StatusProvider{
		taskStorage:          taskStorage,
		workerStatusListener: workerStatusListener,
	}

	statusEnforcerWriter, err := amqpConnector.createWriter("StatusEnforcer",
		AmqpWriterOptions{
			exchangeName: StatusExchangeName,
			exchangeType: amqp.ExchangeFanout,
			durable:      true,
			autoDelete:   false,
			internal:     false,
			noWait:       false,
			args:         nil,
			routingKey:   "",
			mandatory:    false,
			immediate:    false,
			contentType:  "text/plain",
		},
	)
	if err != nil {
		return nil, fmt.Errorf("failed to create status enforcer writer: %v", err)
	}

	statusEnforcer := &StatusEnforcer{
		writer:      statusEnforcerWriter,
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
