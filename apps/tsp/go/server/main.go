package main

import (
	"assfire.org/api/v1/tsp"
	"flag"
	"fmt"
	log "github.com/sirupsen/logrus"
	"google.golang.org/grpc"
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

func main() {
	flag.Parse()
	log.Printf("Starting listening at %s:%d\n", *bindAddress, *port)
	lis, err := net.Listen("tcp", fmt.Sprintf("%s:%d", *bindAddress, *port))
	if err != nil {
		log.Fatalf("Failed to listen: %v", err)
	}
	var opts []grpc.ServerOption
	grpcServer := grpc.NewServer(opts...)
	server, err := createServer(
		*redisEndpoint,
		*redisPort,
		*redisPassword,
		*redisDatabase,
		*rabbitEndpoint,
		*rabbitPort,
		*rabbitUser,
		*rabbitPassword,
	)

	if err != nil {
		log.Fatalf("Error while trying to initilize server: %v", err)
	}

	log.Infoln("Registering server handlers...")
	tsp.RegisterTspServiceServer(grpcServer, server)

	log.Infoln("Starting accepting requests...")
	err = grpcServer.Serve(lis)
	if err != nil {
		log.Fatalf("Server failed with error: %v", err)
	}
}
