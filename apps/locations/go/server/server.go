package main

import (
	"assfire.org/api/v1/locations"
	"context"
	"flag"
	"fmt"
	"go.mongodb.org/mongo-driver/mongo"
	"google.golang.org/grpc"
	"log"
	"math/rand"
	"net"
)

var (
	bindAddress = flag.String("bind-address", "localhost", "Server bind address")
	port        = flag.Int("port", 50051, "Server port")
)

type LocationsServer struct {
	locations.UnimplementedLocationsServiceServer
}

func (server *LocationsServer) GenerateLocations(ctx context.Context, request *locations.GenerateLocationsRequest) (*locations.GenerateLocationsResponse, error) {
	extents := request.GetOptions().GetExtents()
	lonDiff := extents.GetRightLon() - extents.GetLeftLon()
	latDiff := extents.GetTopLat() - extents.GetBottomLat()
	result := make([]*locations.Location, request.GetOptions().GetCount())
	for i := int32(0); i < request.GetOptions().GetCount(); i++ {
		lonOffset := rand.Int31n(lonDiff)
		latOffset := rand.Int31n(latDiff)
		result[i] = &locations.Location{
			EncodedLatitude:  extents.GetLeftLon() + lonOffset,
			EncodedLongitude: extents.GetBottomLat() + latOffset,
		}
	}
	return &locations.GenerateLocationsResponse{
		Locations: result,
	}, nil
}

func destroyMongoClient(ctx context.Context, client *mongo.Client) {
	err := client.Disconnect(ctx)
	if err != nil {
		log.Fatalf("Failed to disconnect from mongo: %v", err)
	}
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
	server := &LocationsServer{}
	log.Println("Registering server handlers...")
	locations.RegisterLocationsServiceServer(grpcServer, server)

	log.Println("Starting accepting requests...")
	err = grpcServer.Serve(lis)

	if err != nil {
		log.Fatalf("Server failed with error: %v", err)
	}
}
