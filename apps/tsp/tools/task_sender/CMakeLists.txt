add_executable(assfire-tsp-task-sender main.cpp)
target_link_libraries(assfire-tsp-task-sender PRIVATE assfire-tsp-grpc assfire-tsp-api assfire-tsp-proto-cpp-translation)