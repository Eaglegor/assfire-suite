package main

import (
  "context"
  "flag"
  "net/http"
  "log"
  "strings"

  "github.com/grpc-ecosystem/grpc-gateway/v2/runtime"
  "google.golang.org/grpc"
  gw "@ASSFIRE_GO_PACKAGE_NAME@"
)

var (
  // command-line options:
  // gRPC server endpoint
  grpcServerEndpoint = flag.String("grpc-server-endpoint",  "localhost:50051", "gRPC server endpoint")
  bindAddress = flag.String("bind-address",  ":8082", "Bind address")
)


// allowCORS allows Cross Origin Resoruce Sharing from any origin.
// Don't do this without consideration in production systems.
func preflightHandler(w http.ResponseWriter, r *http.Request) {
	headers := []string{"Content-Type", "Accept"}
	w.Header().Set("Access-Control-Allow-Headers", strings.Join(headers, ","))
	methods := []string{"GET", "HEAD", "POST", "PUT", "DELETE"}
	w.Header().Set("Access-Control-Allow-Methods", strings.Join(methods, ","))
	log.Print("preflight request for %s", r.URL.Path)
	return
}
func allowCORS(h http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		if origin := r.Header.Get("Origin"); origin != "" {
			w.Header().Set("Access-Control-Allow-Origin", origin)
			if r.Method == "OPTIONS" && r.Header.Get("Access-Control-Request-Method") != "" {
				preflightHandler(w, r)
				return
			}
		}
		h.ServeHTTP(w, r)
	})
}

func run() error {
  log.Print("Starting gRPC proxy")

  ctx := context.Background()
  ctx, cancel := context.WithCancel(ctx)
  defer cancel()

  // Register gRPC server endpoint
  // Note: Make sure the gRPC server is running properly and accessible
  mux := runtime.NewServeMux()
  opts := []grpc.DialOption{grpc.WithInsecure()}

  @ASSFIRE_REGISTER_GRPC_SERVICE_ENDPOINTS@

  // Start HTTP server (and proxy calls to gRPC server endpoint)
  if @ENABLE_CORS@ {
    return http.ListenAndServe(*bindAddress, allowCORS(mux))
  } else {
    return http.ListenAndServe(*bindAddress, mux)
  }
}

func main() {
  flag.Parse()
  //defer log.Flush()

  if err := run(); err != nil {
    log.Fatal(err)
  }
}
