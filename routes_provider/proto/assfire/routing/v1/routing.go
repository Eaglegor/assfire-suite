package main

import (
  "context"
  "flag"
  "net/http"
  "log"

  "github.com/grpc-ecosystem/grpc-gateway/runtime"
  "google.golang.org/grpc"

  gw "assfire.org/routing/v1"
)

var (
  // command-line options:
  // gRPC server endpoint
  grpcServerEndpoint = flag.String("grpc-server-endpoint",  "localhost:50051", "gRPC server endpoint")
  bindAddress = flag.String("bind-address",  ":8082", "Bind address")
)

func run() error {
  log.Print("Starting gRPC proxy")

  ctx := context.Background()
  ctx, cancel := context.WithCancel(ctx)
  defer cancel()

  // Register gRPC server endpoint
  // Note: Make sure the gRPC server is running properly and accessible
  mux := runtime.NewServeMux(runtime.WithMarshalerOption(runtime.MIMEWildcard, &runtime.JSONPb{OrigName: true, EmitDefaults: true}))
  opts := []grpc.DialOption{grpc.WithInsecure()}
  err := gw.RegisterRoutesProviderHandlerFromEndpoint(ctx, mux,  *grpcServerEndpoint, opts)
  if err != nil {
    return err
  }

  // Start HTTP server (and proxy calls to gRPC server endpoint)
  return http.ListenAndServe(*bindAddress, mux)
}

func main() {
  flag.Parse()
  //defer log.Flush()

  if err := run(); err != nil {
    log.Fatal(err)
  }
}