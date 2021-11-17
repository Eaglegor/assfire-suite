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
	tls        = flag.Bool("tls", false, "Connection uses TLS if true, else plain TCP")
	certFile   = flag.String("cert_file", "", "The TLS cert file")
	keyFile    = flag.String("key_file", "", "The TLS key file")
	jsonDBFile = flag.String("json_db_file", "", "A json file containing a list of features")
	port       = flag.Int("port", 50051, "The server port")
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

func formatInProgressKey(taskName string) string {
	return fmt.Sprintf("%s%s%s", "tsp:", taskName, ":in.progress")
}

func (server *tspServer) StartTsp(ctx context.Context, request *tsp.StartTspRequest) (*tsp.StartTspResponse, error) {
	taskId := uuid.New().String()
	log.Printf("Starting new TSP task %s", taskId)
	workerTask := tsp.WorkerTask{TaskId: taskId, Task: request.Task}

	serializedTask, _ := proto.Marshal(&workerTask)
	server.redisClient.Set(ctx, formatTaskKey(taskId), serializedTask, 0)
	server.redisClient.Set(ctx, formatInProgressKey(taskId), 1, 0)

	server.rabbitChannel.Publish(
		"amq.direct",
		server.rabbitTaskQueue.Name,
		false,
		false,
		amqp.Publishing{
			ContentType: "text/plain",
			Body:        serializedTask,
		},
	)

	return &tsp.StartTspResponse{
		TaskId: taskId,
	}, nil
}

func (server *tspServer) PauseTsp(ctx context.Context, request *tsp.PauseTspRequest) (*tsp.PauseTspResponse, error) {
	log.Printf("Pausing TSP task %s", request.TaskId)

	controlSignal := tsp.WorkerControlSignal{
		TaskId:     request.TaskId,
		SignalType: tsp.WorkerControlSignal_WORKER_CONTROL_SIGNAL_TYPE_PAUSE,
	}

	serializedSignal, _ := proto.Marshal(&controlSignal)
	server.rabbitChannel.Publish(
		"amq.topic",
		server.rabbitControlSignalQueue.Name,
		false,
		false,
		amqp.Publishing{
			ContentType: "text/plain",
			Body:        serializedSignal,
		},
	)

	solution_value := server.redisClient.Get(ctx, formatSolutionKey(request.TaskId))
	var tspSolution tsp.TspSolution

	bytes, err := solution_value.Bytes()
	if err != nil {
		return &tsp.PauseTspResponse{
			Solution: nil,
		}, nil
	}
	proto.Unmarshal(bytes, &tspSolution)

	server.redisClient.Set(ctx, formatInProgressKey(request.TaskId), 0, 0)

	return &tsp.PauseTspResponse{
		Solution: &tspSolution,
	}, nil
}

func (server *tspServer) ResumeTsp(ctx context.Context, request *tsp.ResumeTspRequest) (*tsp.ResumeTspResponse, error) {
	log.Printf("Resuming TSP task %s", request.TaskId)

	redisResponse := server.redisClient.Get(ctx, formatTaskKey(request.TaskId))
	if redisResponse.Err() != redis.Nil {
		bytes, _ := redisResponse.Bytes()
		server.rabbitChannel.Publish(
			"amq.direct",
			server.rabbitTaskQueue.Name,
			false,
			false,
			amqp.Publishing{
				ContentType: "text/plain",
				Body:        bytes,
			},
		)

		server.redisClient.Set(ctx, formatInProgressKey(request.TaskId), 1, 0)

		return &tsp.ResumeTspResponse{}, nil
	} else {
		return nil, status.Errorf(codes.NotFound, "Task %s not found", request.TaskId)
	}
}

func (server *tspServer) StopTsp(ctx context.Context, request *tsp.StopTspRequest) (*tsp.StopTspResponse, error) {
	log.Printf("Stopping TSP task %s", request.TaskId)

	controlSignal := tsp.WorkerControlSignal{
		TaskId:     request.TaskId,
		SignalType: tsp.WorkerControlSignal_WORKER_CONTROL_SIGNAL_TYPE_INTERRUPT,
	}

	serializedSignal, _ := proto.Marshal(&controlSignal)
	server.rabbitChannel.Publish(
		"amq.topic",
		server.rabbitControlSignalQueue.Name,
		false,
		false,
		amqp.Publishing{
			ContentType: "text/plain",
			Body:        serializedSignal,
		},
	)

	solution_value := server.redisClient.Get(ctx, formatSolutionKey(request.TaskId))
	var tspSolution tsp.TspSolution

	bytes, err := solution_value.Bytes()
	if err != nil {
		return &tsp.StopTspResponse{
			Solution: nil,
		}, nil
	}
	proto.Unmarshal(bytes, &tspSolution)

	server.redisClient.Set(ctx, formatInProgressKey(request.TaskId), 0, 0)

	return &tsp.StopTspResponse{
		Solution: &tspSolution,
	}, nil
}

func (server *tspServer) GetLatestSolution(ctx context.Context, request *tsp.GetLatestSolutionRequest) (*tsp.GetLatestSolutionResponse, error) {
	log.Printf("Retrieving latest solution for TSP task %s", request.TaskId)

	solution_value := server.redisClient.Get(ctx, formatSolutionKey(request.TaskId))
	var tspSolution tsp.TspSolution

	bytes, err := solution_value.Bytes()
	if err != nil {
		return &tsp.GetLatestSolutionResponse{
			Solution: nil,
		}, nil
	}
	proto.Unmarshal(bytes, &tspSolution)

	return &tsp.GetLatestSolutionResponse{
		Solution: &tspSolution,
	}, nil
}

func (server *tspServer) SubscribeForStatusUpdates(request *tsp.SubscribeForStatusUpdatesRequest, observer tsp.TspService_SubscribeForStatusUpdatesServer) error {
	return status.Errorf(codes.Unimplemented, "method SubscribeForStatusUpdates not implemented")
}

func newServer() *tspServer {
	redisClient := redis.NewClient(&redis.Options{
		Addr:     "172.18.227.52:6379",
		Password: "",
		DB:       0,
	})

	rabbitConnection, _ := amqp.Dial("amqp://guest:guest@localhost:5672/")
	rabbitChannel, _ := rabbitConnection.Channel()

	rabbitTaskQueue, _ := rabbitChannel.QueueDeclare(
		"org.assfire.tsp.worker.task",
		false,
		false,
		false,
		false,
		nil,
	)

	rabbitControlSignalQueue, _ := rabbitChannel.QueueDeclare(
		"org.assfire.tsp.worker.control_signal",
		false,
		false,
		false,
		false,
		nil,
	)

	s := &tspServer{
		redisClient:              redisClient,
		rabbitConnection:         rabbitConnection,
		rabbitChannel:            rabbitChannel,
		rabbitTaskQueue:          rabbitTaskQueue,
		rabbitControlSignalQueue: rabbitControlSignalQueue,
	}

	return s
}

func main() {
	flag.Parse()
	fmt.Printf("Starting server at localhost:%d\n", *port)
	lis, err := net.Listen("tcp", fmt.Sprintf("localhost:%d", *port))
	if err != nil {
		log.Fatalf("failed to listen: %v", err)
	}
	var opts []grpc.ServerOption
	grpcServer := grpc.NewServer(opts...)
	fmt.Println("Registering handlers")
	server := newServer()
	tsp.RegisterTspServiceServer(grpcServer, server)

	defer server.rabbitConnection.Close()
	defer server.rabbitChannel.Close()

	grpcServer.Serve(lis)
}
