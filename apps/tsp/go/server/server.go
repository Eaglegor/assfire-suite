package main

import (
	"assfire.org/api/v1/tsp"
	"context"
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

type tspServer struct {
	tsp.UnimplementedTspServiceServer

	redisClient              *redis.Client
	rabbitConnection         *amqp.Connection
	rabbitChannel            *amqp.Channel
	rabbitTaskQueue          amqp.Queue
	rabbitControlSignalQueue amqp.Queue
}

func formatTaskKey(taskName string) string {
	return fmt.Sprintf("%s%s%s", "tsp:", taskName, ":task")
}

func formatSolutionKey(taskName string) string {
	return fmt.Sprintf("%s%s%s", "tsp:", taskName, ":solution")
}

func publishTask(ctx context.Context, task *tsp.WorkerTask, redisClient *redis.Client) error {
	serializedTask, err := proto.Marshal(task)
	if err == nil {
		result := redisClient.Set(ctx, formatTaskKey(task.TaskId), serializedTask, 0)
		return result.Err()
	} else {
		return err
	}
}

func hasTask(ctx context.Context, taskId string, redisClient *redis.Client) (bool, error) {
	response := redisClient.Exists(ctx, formatTaskKey(taskId))
	return response.Err() == nil, response.Err()
}

func loadSolution(ctx context.Context, taskId string, redisClient *redis.Client) (*tsp.TspSolution, error) {
	response := redisClient.Get(ctx, formatSolutionKey(taskId))
	if response.Err() != nil {
		return nil, response.Err()
	}

	var tspSolution tsp.TspSolution

	bytes, err := response.Bytes()
	if err != nil {
		return nil, err
	}
	err = proto.Unmarshal(bytes, &tspSolution)
	if err != nil {
		return nil, err
	}

	return &tspSolution, nil
}

func removeTask(ctx context.Context, taskId string, redisClient *redis.Client) error {
	return redisClient.Del(ctx, formatTaskKey(taskId)).Err()
}

const (
	START = iota
	STOP  = iota
)

const SIGNAL_EXCHANGE = "amq.topic"
const SIGNAL_QUEUE = "org.assfire.tsp.worker.signal"

const STATUS_QUEUE = "org.assfire.tsp.worker.status"

func sendSignal(taskId string, signal int, rabbitChannel *amqp.Channel) error {

	var signalType tsp.WorkerControlSignal_Type
	if signal == START {
		signalType = tsp.WorkerControlSignal_WORKER_CONTROL_SIGNAL_TYPE_START
	} else if signal == STOP {
		signalType = tsp.WorkerControlSignal_WORKER_CONTROL_SIGNAL_TYPE_STOP
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
		SIGNAL_EXCHANGE,
		SIGNAL_QUEUE,
		false,
		false,
		amqp.Publishing{
			ContentType: "text/plain",
			Body:        message,
		},
	)

	if err != nil {
		return err
	}

	return nil
}

func sendStartSignal(taskId string, rabbitChannel *amqp.Channel) error {
	return sendSignal(taskId, START, rabbitChannel)
}

func sendStopSignal(taskId string, rabbitChannel *amqp.Channel) error {
	return sendSignal(taskId, STOP, rabbitChannel)
}

func (server *tspServer) StartTsp(ctx context.Context, request *tsp.StartTspRequest) (*tsp.StartTspResponse, error) {
	taskId := uuid.New().String()
	log.Printf("Starting new TSP task %s", taskId)

	log.Printf("Publishing task %s", taskId)
	err := publishTask(ctx, &tsp.WorkerTask{TaskId: taskId, Task: request.Task}, server.redisClient)
	if err != nil {
		log.Printf("Failed to publish task %s: %s", taskId, err.Error())
		return nil, status.Errorf(status.Code(err), "Failed to publish task %s", taskId)
	}

	log.Printf("Sending start signal %s", taskId)
	err = sendStartSignal(taskId, server.rabbitChannel)
	if err != nil {
		log.Printf("Failed to send start signal for task %s: %s", taskId, err.Error())
		return nil, status.Errorf(status.Code(err), "Failed to start task %s", taskId)
	}

	return &tsp.StartTspResponse{
		TaskId: taskId,
	}, nil
}

func (server *tspServer) PauseTsp(ctx context.Context, request *tsp.PauseTspRequest) (*tsp.PauseTspResponse, error) {
	log.Printf("Pausing TSP task %s", request.TaskId)

	if exists, err := hasTask(ctx, request.TaskId, server.redisClient); !exists {
		log.Printf("Failed to find task %s: %s", request.TaskId, err.Error())
		if err == redis.Nil {
			return nil, status.Errorf(codes.NotFound, "Failed to pause task %s - task not found", request.TaskId)
		} else {
			return nil, status.Errorf(codes.Internal, "Failed to pause task %s", request.TaskId)
		}
	}

	err := sendStopSignal(request.TaskId, server.rabbitChannel)
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

func (server *tspServer) ResumeTsp(ctx context.Context, request *tsp.ResumeTspRequest) (*tsp.ResumeTspResponse, error) {
	log.Printf("Resuming TSP task %s", request.TaskId)

	if exists, err := hasTask(ctx, request.TaskId, server.redisClient); !exists {
		log.Printf("Failed to find task %s: %s", request.TaskId, err.Error())
		if err == redis.Nil {
			return nil, status.Errorf(codes.NotFound, "Failed to resume task %s - task not found", request.TaskId)
		} else {
			return nil, status.Errorf(codes.Internal, "Failed to resume task %s", request.TaskId)
		}
	}

	err := sendStartSignal(request.TaskId, server.rabbitChannel)
	if err != nil {
		log.Printf("Failed to send start signal for task %s: %s", request.TaskId, err.Error())
		return nil, status.Errorf(status.Code(err), "Failed to resume task %s", request.TaskId)
	}

	return &tsp.ResumeTspResponse{}, nil
}

func (server *tspServer) StopTsp(ctx context.Context, request *tsp.StopTspRequest) (*tsp.StopTspResponse, error) {
	log.Printf("Stopping TSP task %s", request.TaskId)

	if exists, err := hasTask(ctx, request.TaskId, server.redisClient); !exists {
		log.Printf("Failed to find task %s: %s", request.TaskId, err.Error())
		if err == redis.Nil {
			return nil, status.Errorf(codes.NotFound, "Failed to stop task %s - task not found", request.TaskId)
		} else {
			return nil, status.Errorf(codes.Internal, "Failed to stop task %s", request.TaskId)
		}
	}

	err := sendStopSignal(request.TaskId, server.rabbitChannel)
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

func (server *tspServer) GetLatestSolution(ctx context.Context, request *tsp.GetLatestSolutionRequest) (*tsp.GetLatestSolutionResponse, error) {
	log.Printf("Retrieving latest solution for TSP task %s", request.TaskId)

	solution, err := loadSolution(ctx, request.TaskId, server.redisClient)
	if err == redis.Nil {
		return &tsp.GetLatestSolutionResponse{}, nil
	} else if err != nil {
		log.Printf("Failed to lookup for solution for task %s: %s", request.TaskId, err.Error())
		return nil, status.Errorf(status.Code(err), "Failed to retrieve solution for task %s", request.TaskId)
	}

	return &tsp.GetLatestSolutionResponse{
		Solution: solution,
	}, nil
}

func parseStatusUpdate(msg []byte) (*tsp.WorkerTspStatusUpdate, error) {
	var result *tsp.WorkerTspStatusUpdate
	err := proto.Unmarshal(msg, result)
	if err != nil {
		return nil, err
	}
	return result, nil
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
	default:
		return tsp.TspStatusUpdate_TSP_STATUS_UPDATE_TYPE_UNKNOWN
	}
}

func isTerminalState(update tsp.WorkerTspStatusUpdate_Type) bool {
	return update == tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_ERROR ||
		update == tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_FINISHED
}

func (server *tspServer) SubscribeForStatusUpdates(request *tsp.SubscribeForStatusUpdatesRequest, observer tsp.TspService_SubscribeForStatusUpdatesServer) error {
	updates, err := server.rabbitChannel.Consume(
		STATUS_QUEUE,
		"",
		true,
		false,
		false,
		false,
		nil,
	)
	if err != nil {
		log.Printf("Failed to subscribe for status updates for task %s: %s", request.TaskId, err.Error())
		return status.Errorf(status.Code(err), "Failed to subscribe for updates for task %s", request.TaskId)
	}
	for updateMessage := range updates {
		update, err := parseStatusUpdate(updateMessage.Body)
		if err != nil {
			log.Printf("Failed to process status update for task %s: %s", request.TaskId, err.Error())
		} else {
			if update.GetTaskId() == request.TaskId {
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
					return nil
				}
			}
		}
	}
	return nil
}

func newServer() *tspServer {
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

	rabbitChannel, err := rabbitConnection.Channel()

	if err != nil {
		log.Fatalf("Couldn't initialize rabbitMQ channel: %s", err.Error())
	}

	_, err = rabbitChannel.QueueDeclare(
		SIGNAL_QUEUE,
		false,
		false,
		false,
		false,
		nil,
	)

	if err != nil {
		log.Fatalf("Couldn't initialize rabbitMQ queue %s: %s", SIGNAL_QUEUE, err.Error())
	}

	_, err = rabbitChannel.QueueDeclare(
		STATUS_QUEUE,
		false,
		false,
		false,
		false,
		nil,
	)

	if err != nil {
		log.Fatalf("Couldn't initialize rabbitMQ queue %s: %s", STATUS_QUEUE, err.Error())
	}

	s := &tspServer{
		redisClient:      redisClient,
		rabbitConnection: rabbitConnection,
		rabbitChannel:    rabbitChannel,
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

	defer func(rabbitConnection *amqp.Connection) {
		log.Println("Closing rabbit MQ connection...")
		err := rabbitConnection.Close()
		if err != nil {
			log.Fatalf("Failed to close Rabbit MQ connection gracefully: %v", err)
		}
	}(server.rabbitConnection)
	defer func(rabbitChannel *amqp.Channel) {
		log.Println("Closing rabbit MQ channel...")
		err := rabbitChannel.Close()
		if err != nil {
			log.Fatalf("Failed to close Rabbit MQ channel gracefully: %v", err)
		}
	}(server.rabbitChannel)

	log.Println("Starting accepting requests...")
	err = grpcServer.Serve(lis)
	if err != nil {
		log.Fatalf("Server failed with error: %v", err)
	}
}
