package main

import (
	"assfire.org/api/v1/locations"
	"context"
	"flag"
	"fmt"
	"go.mongodb.org/mongo-driver/bson"
	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"
	"go.mongodb.org/mongo-driver/mongo/readpref"
	"google.golang.org/grpc"
	"log"
	"net"
	"time"
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

type LocationsServer struct {
	locations.UnimplementedLocationsServiceServer

	mongoClient *mongo.Client
}

func main() {
	flag.Parse()
	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()
	log.Printf("Trying to connect to mongodb")
	client, err := mongo.Connect(ctx, options.Client().ApplyURI("mongodb://localhost:27017"))
	if err != nil {
		log.Fatalf("Failed to connect to mongodb: %v", err)
	}

	err = client.Ping(ctx, readpref.Primary())
	if err != nil {
		log.Fatalf("Failed to connect to mongodb: %v", err)
	}

	collection := client.Database("testing").Collection("numbers")
	one, err := collection.InsertOne(ctx, bson.D{{"name", "pi"}, {"value", 3.14}})
	if err != nil {
		log.Fatalf("Failed to insert: %v", err)
	}

	log.Printf("Inserted: %v", one.InsertedID)

	defer func() {
		if err = client.Disconnect(ctx); err != nil {
			log.Fatalf("Failed to disconnect from mongo: %v", err)
		}
	}()

	log.Printf("Starting listening at %s:%d\n", *bindAddress, *port)
	lis, err := net.Listen("tcp", fmt.Sprintf("%s:%d", *bindAddress, *port))
	if err != nil {
		log.Fatalf("Failed to listen: %v", err)
	}
	var opts []grpc.ServerOption
	grpcServer := grpc.NewServer(opts...)
	log.Println("Creating server...")
	server := &LocationsServer{
		mongoClient: client,
	}
	log.Println("Registering server handlers...")
	locations.RegisterLocationsServiceServer(grpcServer, server)

	log.Println("Starting accepting requests...")
	err = grpcServer.Serve(lis)

	if err != nil {
		log.Fatalf("Server failed with error: %v", err)
	}
}
