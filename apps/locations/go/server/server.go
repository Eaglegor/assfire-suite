package main

import (
	"assfire.org/api/v1/locations"
	"context"
	"flag"
	"fmt"
	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"
	"go.mongodb.org/mongo-driver/mongo/readpref"
	"google.golang.org/grpc"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/status"
	"log"
	"math/rand"
	"net"
	"time"
)

var (
	bindAddress = flag.String("bind-address", "localhost", "Server bind address")
	port        = flag.Int("port", 50051, "Server port")
	mongoDbHost = flag.String("mongodb-host", "localhost", "MongoDB host")
	mongoDbPort = flag.Int("mongodb-port", 27017, "MongoDB port")
)

type LocationsServer struct {
	locations.UnimplementedLocationsServiceServer

	mongoClient *mongo.Client
}

func (server *LocationsServer) GetLocations(ctx context.Context, request *locations.GetLocationsRequest) (*locations.GetLocationsResponse, error) {
	return nil, status.Errorf(codes.Unimplemented, "method GetLocations not implemented")
}
func (server *LocationsServer) SaveLocations(ctx context.Context, request *locations.SaveLocationsRequest) (*locations.SaveLocationsResponse, error) {
	return nil, status.Errorf(codes.Unimplemented, "method SaveLocations not implemented")
}
func (server *LocationsServer) RemoveLocations(ctx context.Context, request *locations.RemoveLocationsRequest) (*locations.RemoveLocationsResponse, error) {
	return nil, status.Errorf(codes.Unimplemented, "method RemoveLocations not implemented")
}
func (server *LocationsServer) FindLocations(ctx context.Context, request *locations.FindLocationsRequest) (*locations.FindLocationsResponse, error) {
	return nil, status.Errorf(codes.Unimplemented, "method FindLocations not implemented")
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

func (server *LocationsServer) ResolveAddresses(ctx context.Context, request *locations.ResolveAddressesRequest) (*locations.ResolveAddressesResponse, error) {
	return nil, status.Errorf(codes.Unimplemented, "method ResolveAddresses not implemented")
}

func createMongoClient(ctx context.Context) *mongo.Client {
	mongoConnectionString := fmt.Sprintf("mongodb://%s:%d", *mongoDbHost, *mongoDbPort)
	log.Printf("Trying to connect to mongoDB at %s", mongoConnectionString)
	client, err := mongo.Connect(ctx, options.Client().ApplyURI(mongoConnectionString))
	if err != nil {
		log.Fatalf("Failed to connect to mongoDB: %v", err)
	}

	log.Printf("Checking if mongoDB client connected...")
	err = client.Ping(ctx, readpref.Primary())
	if err != nil {
		log.Fatalf("Failed to connect to mongoDB: %v", err)
	}
	return client
}

func destroyMongoClient(ctx context.Context, client *mongo.Client) {
	err := client.Disconnect(ctx)
	if err != nil {
		log.Fatalf("Failed to disconnect from mongo: %v", err)
	}
}

func main() {
	flag.Parse()

	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	mongoClient := createMongoClient(ctx)
	defer destroyMongoClient(ctx, mongoClient)

	log.Printf("Starting listening at %s:%d\n", *bindAddress, *port)
	lis, err := net.Listen("tcp", fmt.Sprintf("%s:%d", *bindAddress, *port))
	if err != nil {
		log.Fatalf("Failed to listen: %v", err)
	}
	var opts []grpc.ServerOption
	grpcServer := grpc.NewServer(opts...)
	log.Println("Creating server...")
	server := &LocationsServer{
		mongoClient: mongoClient,
	}
	log.Println("Registering server handlers...")
	locations.RegisterLocationsServiceServer(grpcServer, server)

	log.Println("Starting accepting requests...")
	err = grpcServer.Serve(lis)

	if err != nil {
		log.Fatalf("Server failed with error: %v", err)
	}
}
