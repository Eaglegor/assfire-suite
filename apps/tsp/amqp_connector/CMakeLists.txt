find_package(spdlog CONFIG REQUIRED)
find_package(rabbitmq-c CONFIG REQUIRED)

add_library(assfire-util-amqp-connector
        assfire/util/amqp/RabbitMqConnector.hpp
        assfire/util/amqp/RabbitMqConnector.cpp
        )

target_link_libraries(assfire-util-amqp-connector PUBLIC rabbitmq::rabbitmq rabbitmq::rabbitmq-static PRIVATE spdlog::spdlog)
target_include_directories(assfire-util-amqp-connector PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})