package main

import (
	"assfire.org/api/v1/tsp"
	"context"
	"flag"
	"fmt"
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
}

func (server* tspServer) StartTsp(context.Context, *tsp.StartTspRequest) (*tsp.StartTspResponse, error) {
	fmt.Println("Starting TSP server")
	return nil, status.Errorf(codes.Unimplemented, "method StartTsp not implemented")
}

func (server* tspServer) PauseTsp(context.Context, *tsp.PauseTspRequest) (*tsp.PauseTspResponse, error) {
	return nil, status.Errorf(codes.Unimplemented, "method PauseTsp not implemented")
}

func (server* tspServer) ResumeTsp(context.Context, *tsp.ResumeTspRequest) (*tsp.ResumeTspResponse, error) {
	return nil, status.Errorf(codes.Unimplemented, "method ResumeTsp not implemented")
}

func (server* tspServer) StopTsp(context.Context, *tsp.StopTspRequest) (*tsp.StopTspResponse, error) {
	return nil, status.Errorf(codes.Unimplemented, "method StopTsp not implemented")
}

func (server* tspServer) GetLatestSolution(context.Context, *tsp.GetLatestSolutionRequest) (*tsp.GetLatestSolutionResponse, error) {
	return nil, status.Errorf(codes.Unimplemented, "method GetLatestSolution not implemented")
}

func (server* tspServer) SubscribeForStatusUpdates(*tsp.SubscribeForStatusUpdatesRequest, tsp.TspService_SubscribeForStatusUpdatesServer) error {
	return status.Errorf(codes.Unimplemented, "method SubscribeForStatusUpdates not implemented")
}

func newServer() *tspServer {
	s := &tspServer{}
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
	tsp.RegisterTspServiceServer(grpcServer, newServer())
	grpcServer.Serve(lis)
}
