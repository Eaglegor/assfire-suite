package main

import (
	"assfire.org/api/v1/tsp"
	"context"
	"errors"
	"flag"
	"fmt"
	"github.com/go-redis/redis/v8"
	"github.com/golang/protobuf/proto"
	"github.com/google/uuid"
	"github.com/streadway/amqp"
	"google.golang.org/grpc"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/status"
	"log"
	"net"
)

var (
	bindAddress    = flag.String("bind-address", "localhost", "Server bind address")
	port           = flag.Int("port", 50051, "Server port")
	redisEndpoint  = flag.String("redis-endpoint", "localhost", "Redis endpoint")
	redisPort      = flag.Int("redis-port", 6379, "Redis port")
	redisPassword  = flag.String("redis-password", "", "Redis password")
	redisDatabase  = flag.Int("redis-database", 0, "Redis database id")
	rabbitEndpoint = flag.String("rabbit-endpoint", "localhost", "Rabbit MQ endpoint")
	rabbitPort     = flag.Int("rabbit-port", 5672, "Rabbit MQ port")
	rabbitUser     = flag.String("rabbit-user", "guest", "Rabbit MQ user")
	rabbitPassword = flag.String("rabbit-password", "guest", "Rabbit MQ password")
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

	TaskQueue      = "assfire.tsp.worker.task"
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

type TspServer struct {
	tsp.UnimplementedTspServiceServer

	redisClient              *redis.Client
	rabbitConnection         *amqp.Connection
	taskRabbitChannel        *amqp.Channel
	statusRabbitChannel      *amqp.Channel
	signalRabbitChannel      *amqp.Channel
	rabbitTaskQueue          amqp.Queue
	rabbitControlSignalQueue amqp.Queue
}

func publishTask(ctx context.Context, taskId string, task *tsp.TspTask, redisClient *redis.Client) error {
	serializedTask, err := proto.Marshal(task)
	if err == nil {
		result := redisClient.Set(ctx, taskKey(taskId), serializedTask, 0)
		return result.Err()
	} else {
		return err
	}
}

func getTaskStatus(ctx context.Context, taskId string, redisClient *redis.Client) (tsp.WorkerTspStatusUpdate_Type, error) {
	response := redisClient.Get(ctx, statusKey(taskId))
	if response.Err() != nil {
		log.Printf("Failed to determine current status for task %s: %v", taskId, response.Err())
		return tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_UNKNOWN, errors.New("failed to determine current task status")
	}
	code := tsp.WorkerTspStatusUpdate_Type_value[response.Val()]
	return tsp.WorkerTspStatusUpdate_Type(code), nil
}

func isResumeable(ctx context.Context, taskId string, redisClient *redis.Client) (bool, error) {
	taskStatus, err := getTaskStatus(ctx, taskId, redisClient)
	if err != nil {
		log.Printf("Failed to determine task resumeability %s: %v", taskId, err)
		return false, errors.New("failed to determine task resumeability")
	}
	if taskStatus == tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_IN_PROGRESS {
		return false, errors.New("task is already in progress")
	}
	if taskStatus == tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_FINISHED {
		return false, errors.New("task is already finished")
	}

	return true, nil
}

func hasTask(ctx context.Context, taskId string, redisClient *redis.Client) bool {
	response := redisClient.Exists(ctx, taskKey(taskId))
	if response.Err() != nil {
		log.Printf("Failed to determine if task %s exists: %v", taskId, response.Err())
	}
	return response.Err() == nil && response.Val() != 0
}

func loadSolution(ctx context.Context, taskId string, redisClient *redis.Client) (*tsp.TspSolution, error) {
	response := redisClient.Get(ctx, solutionKey(taskId))
	if response.Err() != nil {
		return nil, response.Err()
	}

	var tspSolution tsp.TspSolution

	err := proto.Unmarshal([]byte(response.Val()), &tspSolution)
	if err != nil {
		return nil, err
	}

	log.Printf("Solution for %s: %v", taskId, tspSolution.OptimizedSequence)

	return &tspSolution, nil
}

func removeTask(ctx context.Context, taskId string, redisClient *redis.Client) error {
	return redisClient.Del(ctx, taskKey(taskId)).Err()
}

func sendSignal(taskId string, signal int, rabbitChannel *amqp.Channel) error {

	var signalType tsp.WorkerControlSignal_Type
	if signal == InterruptSignal {
		signalType = tsp.WorkerControlSignal_WORKER_CONTROL_SIGNAL_TYPE_INTERRUPT
	} else if signal == PauseSignal {
		signalType = tsp.WorkerControlSignal_WORKER_CONTROL_SIGNAL_TYPE_PAUSE
	}

	signalObject := tsp.WorkerControlSignal{
		TaskId:     taskId,
		SignalType: signalType,
	}

	message, err := proto.Marshal(&signalObject)
	if err != nil {
		return err
	}

	err = rabbitChannel.Publish(
		InterruptExchange,
		taskId,
		false,
		false,
		amqp.Publishing{
			ContentType: "text/plain",
			Body:        message,
		},
	)

	return err
}

func sendStartSignal(taskId string, rabbitChannel *amqp.Channel) error {
	task := tsp.WorkerTask{TaskId: taskId}
	message, err := proto.Marshal(&task)

	if err != nil {
		return err
	}

	err = rabbitChannel.Publish(
		TaskExchange,
		TaskQueue,
		true,
		false,
		amqp.Publishing{
			ContentType: "text/plain",
			Body:        message,
		})

	return err
}

func sendPauseSignal(taskId string, rabbitChannel *amqp.Channel) error {
	return sendSignal(taskId, PauseSignal, rabbitChannel)
}

func sendInterruptSignal(taskId string, rabbitChannel *amqp.Channel) error {
	return sendSignal(taskId, InterruptSignal, rabbitChannel)
}

func (server *TspServer) StartTsp(ctx context.Context, request *tsp.StartTspRequest) (*tsp.StartTspResponse, error) {
	taskId := uuid.New().String()
	log.Printf("Starting new TSP task %s", taskId)

	log.Printf("Publishing task %s", taskId)
	err := publishTask(ctx, taskId, request.Task, server.redisClient)
	if err != nil {
		log.Printf("Failed to publish task %s: %s", taskId, err.Error())
		return nil, status.Errorf(status.Code(err), "Failed to publish task %s", taskId)
	}

	log.Printf("Sending start signal %s", taskId)
	err = sendStartSignal(taskId, server.taskRabbitChannel)
	if err != nil {
		log.Printf("Failed to send start signal for task %s: %s", taskId, err.Error())
		return nil, status.Errorf(status.Code(err), "Failed to start task %s", taskId)
	}

	return &tsp.StartTspResponse{
		TaskId: taskId,
	}, nil
}

func (server *TspServer) PauseTsp(ctx context.Context, request *tsp.PauseTspRequest) (*tsp.PauseTspResponse, error) {
	log.Printf("Pausing TSP task %s", request.TaskId)

	if exists := hasTask(ctx, request.TaskId, server.redisClient); !exists {
		log.Printf("Failed to find task %s", request.TaskId)
		return nil, status.Errorf(codes.NotFound, "%s - task not found", request.TaskId)
	}

	err := sendPauseSignal(request.TaskId, server.signalRabbitChannel)
	if err != nil {
		log.Printf("Failed to send stop signal for task %s: %s", request.TaskId, err.Error())
		return nil, status.Errorf(status.Code(err), "Failed to pause task %s", request.TaskId)
	}

	solution, err := loadSolution(ctx, request.TaskId, server.redisClient)
	if err == redis.Nil {
		return &tsp.PauseTspResponse{}, nil
	} else if err != nil {
		log.Printf("Failed to lookup for solution for task %s: %s", request.TaskId, err.Error())
		return &tsp.PauseTspResponse{}, nil
	}

	return &tsp.PauseTspResponse{
		Solution: solution,
	}, nil
}

func (server *TspServer) ResumeTsp(ctx context.Context, request *tsp.ResumeTspRequest) (*tsp.ResumeTspResponse, error) {
	log.Printf("Resuming TSP task %s", request.TaskId)

	if exists := hasTask(ctx, request.TaskId, server.redisClient); !exists {
		log.Printf("Failed to find task %s", request.TaskId)
		return nil, status.Errorf(codes.NotFound, "%s - task not found", request.TaskId)
	}

	if resumeable, err := isResumeable(ctx, request.TaskId, server.redisClient); !resumeable {
		log.Printf("Task %s can't be resumed: %v", request.TaskId, err)
		return nil, status.Errorf(codes.AlreadyExists, "%s - task can't be resumed: %v", request.TaskId, err)
	}

	err := sendStartSignal(request.TaskId, server.signalRabbitChannel)
	if err != nil {
		log.Printf("Failed to send start signal for task %s: %v", request.TaskId, err)
		return nil, status.Errorf(status.Code(err), "Failed to resume task %s", request.TaskId)
	}

	return &tsp.ResumeTspResponse{}, nil
}

func (server *TspServer) StopTsp(ctx context.Context, request *tsp.StopTspRequest) (*tsp.StopTspResponse, error) {
	log.Printf("Stopping TSP task %s", request.TaskId)

	if exists := hasTask(ctx, request.TaskId, server.redisClient); !exists {
		log.Printf("Failed to find task %s", request.TaskId)
		return nil, status.Errorf(codes.NotFound, "%s - task not found", request.TaskId)
	}

	taskStatus, err := getTaskStatus(ctx, request.TaskId, server.redisClient)
	if err == nil && taskStatus == tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_PAUSED {
		log.Printf("Sending forced interrupt signal for paused task %s", request.TaskId)
		server.redisClient.Set(ctx, statusKey(request.TaskId), tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_INTERRUPTED.String(), 0)

		forcedStatusUpdate := &tsp.WorkerTspStatusUpdate{
			TaskId: request.TaskId,
			Type:   tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_INTERRUPTED,
		}
		bytes, err := proto.Marshal(forcedStatusUpdate)
		if err == nil {
			err = server.statusRabbitChannel.Publish(
				StatusExchangeName,
				request.TaskId,
				false,
				false,
				amqp.Publishing{
					ContentType: "text/plain",
					Body:        bytes,
				},
			)
		}
	}

	err = sendInterruptSignal(request.TaskId, server.signalRabbitChannel)
	if err != nil {
		log.Printf("Failed to send stop signal for task %s: %s", request.TaskId, err.Error())
		return nil, status.Errorf(status.Code(err), "Failed to stop task %s", request.TaskId)
	}

	solution, err := loadSolution(ctx, request.TaskId, server.redisClient)
	if err == redis.Nil {
		return &tsp.StopTspResponse{}, nil
	} else if err != nil {
		log.Printf("Failed to lookup for solution for task %s: %s", request.TaskId, err.Error())
		return &tsp.StopTspResponse{}, nil
	}

	err = removeTask(ctx, request.TaskId, server.redisClient)
	if err != nil {
		log.Printf("Failed to remove task %s: %s", request.TaskId, err.Error())
		return &tsp.StopTspResponse{}, nil
	}

	return &tsp.StopTspResponse{
		Solution: solution,
	}, nil
}

func (server *TspServer) GetLatestSolution(ctx context.Context, request *tsp.GetLatestSolutionRequest) (*tsp.GetLatestSolutionResponse, error) {
	log.Printf("Retrieving latest solution for TSP task %s", request.TaskId)

	solution, err := loadSolution(ctx, request.TaskId, server.redisClient)
	if err == redis.Nil {
		return nil, status.Errorf(codes.NotFound, "Failed to retrieve solution for task %s", request.TaskId)
	}

	return &tsp.GetLatestSolutionResponse{
		Solution: solution,
	}, nil
}

func parseStatusUpdate(msg []byte) (*tsp.WorkerTspStatusUpdate, error) {
	var result tsp.WorkerTspStatusUpdate
	err := proto.Unmarshal(msg, &result)
	if err != nil {
		return nil, err
	}
	return &result, nil
}

func convertStatusUpdateType(workerType tsp.WorkerTspStatusUpdate_Type) tsp.TspStatusUpdate_Type {
	switch workerType {
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

func isTerminalState(update tsp.WorkerTspStatusUpdate_Type) bool {
	return update == tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_ERROR ||
		update == tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_FINISHED ||
		update == tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_INTERRUPTED ||
		update == tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_UNKNOWN
}

func retrieveLatestStatus(ctx context.Context, taskId string, redisClient *redis.Client, c chan tsp.WorkerTspStatusUpdate_Type) {
	taskStatus, err := getTaskStatus(ctx, taskId, redisClient)
	if err != nil {
		log.Printf("Can't retrieve latest status for task %s: %v", taskId, err)
		return
	}
	c <- taskStatus
}

func (server *TspServer) SubscribeForStatusUpdates(request *tsp.SubscribeForStatusUpdatesRequest, observer tsp.TspService_SubscribeForStatusUpdatesServer) error {
	log.Printf("Subscribing for status updates for TSP task %s", request.TaskId)

	queue, err := server.statusRabbitChannel.QueueDeclare(
		"",
		false,
		true,
		false,
		false,
		nil,
	)
	if err != nil {
		log.Printf("Couldn't declare temporary rabbitMQ queue %s: %s", queue.Name, err.Error())
		return status.Errorf(status.Code(err), "Couldn't declare temporary rabbitMQ status queue for task %s", request.TaskId)
	}

	err = server.statusRabbitChannel.QueueBind(
		queue.Name,
		request.TaskId,
		StatusExchangeName,
		false,
		nil)

	consumerId := uuid.NewString()

	updates, err := server.statusRabbitChannel.Consume(
		queue.Name,
		consumerId,
		false,
		false,
		false,
		false,
		nil,
	)
	if err != nil {
		log.Printf("Failed to subscribe for status updates for task %s: %s", request.TaskId, err.Error())
		return status.Errorf(status.Code(err), "Failed to subscribe for updates for task %s", request.TaskId)
	}

	latestStatusChan := make(chan tsp.WorkerTspStatusUpdate_Type)
	go retrieveLatestStatus(observer.Context(), request.TaskId, server.redisClient, latestStatusChan)

	defer func(statusRabbitChannel *amqp.Channel, consumer string, noWait bool) {
		err := statusRabbitChannel.Cancel(consumer, noWait)
		if err != nil {
			log.Printf("Failed to cancel AMQP subscription to %s: %v", queue.Name, err)
		}
	}(server.statusRabbitChannel, consumerId, false)

	for {
		select {
		case latestStatusType := <-latestStatusChan:
			log.Printf("Retrieved latest status for task %s: %v", request.TaskId, latestStatusType)
			err := observer.Send(&tsp.SubscribeForStatusUpdatesResponse{
				StatusUpdate: &tsp.TspStatusUpdate{
					Type: convertStatusUpdateType(latestStatusType),
				},
			})
			if err != nil {
				log.Printf("Failed to publish status update for task %s: %s", request.TaskId, err.Error())
				return status.Errorf(status.Code(err), "Failed to publish status update for task %s: %s", request.TaskId, err.Error())
			}
			if isTerminalState(latestStatusType) {
				log.Printf("Terminal state received for %s", request.TaskId)
				return nil
			}
		case updateMessage := <-updates:
			update, err := parseStatusUpdate(updateMessage.Body)
			if err != nil {
				log.Printf("Failed to process status update for task %s: %s", request.TaskId, err.Error())
			} else {
				if update.GetTaskId() == request.TaskId {
					log.Printf("Got status update for task %s: %v", request.TaskId, convertStatusUpdateType(update.Type))
					err := observer.Send(&tsp.SubscribeForStatusUpdatesResponse{
						StatusUpdate: &tsp.TspStatusUpdate{
							NewSolutionCost:             update.Cost,
							NewSolutionValidationResult: update.ValidationResult,
							Type:                        convertStatusUpdateType(update.Type),
						},
					})
					if err != nil {
						log.Printf("Failed to publish status update for task %s: %s", request.TaskId, err.Error())
						return status.Errorf(status.Code(err), "Failed to publish status update for task %s: %s", request.TaskId, err.Error())
					}
					if isTerminalState(update.Type) {
						log.Printf("Terminal state received for %s", request.TaskId)
						return nil
					}
					err = server.statusRabbitChannel.Ack(updateMessage.DeliveryTag, false)
					if err != nil {
						log.Printf("Failed to ack rabbit message with status update for %s: %s", request.TaskId, err.Error())
					}
				}
			}
		case <-observer.Context().Done():
			log.Printf("gRPC call is finished for %s", request.TaskId)
			return nil
		}
	}
}

func declareInterruptExchange(rabbitChannel *amqp.Channel) {
	err := rabbitChannel.ExchangeDeclare(
		InterruptExchange,
		amqp.ExchangeFanout,
		false,
		false,
		false,
		false,
		nil,
	)
	if err != nil {
		log.Fatalf("Failed to declare interrupt AMQP exchange %s: %v", InterruptExchange, err)
	}
}

func declareTaskQueue(rabbitChannel *amqp.Channel) {
	_, err := rabbitChannel.QueueDeclare(
		TaskQueue,
		false,
		false,
		false,
		false,
		nil,
	)
	if err != nil {
		log.Fatalf("Couldn't declare task AMQP queue %s: %v", TaskQueue, err)
	}
}

func declareStatusExchange(rabbitChannel *amqp.Channel) {
	err := rabbitChannel.ExchangeDeclare(
		StatusExchangeName,
		amqp.ExchangeFanout,
		false,
		true,
		false,
		false,
		nil,
	)
	if err != nil {
		log.Fatalf("Failed to declare tsp status AMQP exchange %s: %v", StatusExchangeName, err)
	}
}

func newServer() *TspServer {
	redisClient := redis.NewClient(&redis.Options{
		Addr:     fmt.Sprintf("%s:%d", *redisEndpoint, *redisPort),
		Password: *redisPassword,
		DB:       *redisDatabase,
	})

	rabbitConnection, err := amqp.Dial(fmt.Sprintf("amqp://%s:%s@%s:%d",
		*rabbitUser,
		*rabbitPassword,
		*rabbitEndpoint,
		*rabbitPort))
	if err != nil {
		log.Fatalf("Couldn't initialize rabbitMQ connection: %s", err.Error())
	}

	taskRabbitChannel, err := rabbitConnection.Channel()
	if err != nil {
		log.Fatalf("Couldn't initialize task rabbitMQ channel: %s", err.Error())
	}
	singalRabbitChannel, err := rabbitConnection.Channel()
	if err != nil {
		log.Fatalf("Couldn't initialize signal rabbitMQ channel: %s", err.Error())
	}
	statusRabbitChannel, err := rabbitConnection.Channel()
	if err != nil {
		log.Fatalf("Couldn't initialize status rabbitMQ channel: %s", err.Error())
	}

	declareInterruptExchange(singalRabbitChannel)
	declareTaskQueue(taskRabbitChannel)
	declareStatusExchange(statusRabbitChannel)

	s := &TspServer{
		redisClient:         redisClient,
		rabbitConnection:    rabbitConnection,
		taskRabbitChannel:   taskRabbitChannel,
		signalRabbitChannel: singalRabbitChannel,
		statusRabbitChannel: statusRabbitChannel,
	}

	return s
}

func main() {
	flag.Parse()
	log.Printf("Starting listening at %s:%d\n", *bindAddress, *port)
	lis, err := net.Listen("tcp", fmt.Sprintf("%s:%d", *bindAddress, *port))
	if err != nil {
		log.Fatalf("Failed to listen: %v", err)
	}
	var opts []grpc.ServerOption
	grpcServer := grpc.NewServer(opts...)
	log.Println("Creating server...")
	server := newServer()
	log.Println("Registering server handlers...")
	tsp.RegisterTspServiceServer(grpcServer, server)

	log.Println("Starting accepting requests...")
	err = grpcServer.Serve(lis)
	if err != nil {
		log.Fatalf("Server failed with error: %v", err)
	}
}
