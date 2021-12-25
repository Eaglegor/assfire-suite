set(HEADERS
        assfire/util/amqp/AmqpConnection.hpp
        assfire/util/amqp/AmqpChannel.hpp
        assfire/util/amqp/AmqpChannelPool.hpp
        assfire/util/amqp/AmqpConnectionOpts.hpp
        assfire/util/amqp/AmqpExchangeOpts.hpp
        assfire/util/amqp/AmqpQueueOpts.hpp
        assfire/util/amqp/AmqpQueueBinding.hpp
        assfire/util/amqp/AmqpPublisher.hpp
        assfire/util/amqp/AmqpPublisherOpts.hpp
        assfire/util/amqp/AmqpConsumer.hpp
        assfire/util/amqp/AmqpConsumerOpts.hpp
        assfire/util/amqp/AmqpError.hpp
        assfire/util/amqp/AmqpConnectionPool.hpp
        assfire/util/amqp/AmqpDelivery.hpp
        assfire/util/amqp/AmqpSubscriptionOpts.hpp
        )

set(SOURCES
        assfire/util/amqp/AmqpConnection.cpp
        assfire/util/amqp/AmqpChannel.cpp
        assfire/util/amqp/AmqpConnectionPool.cpp
        )

find_package(rabbitmq-c CONFIG REQUIRED)
find_package(crossguid CONFIG REQUIRED)

add_library(assfire-amqp-connector ${SOURCES} ${HEADERS})

if (WIN32)
    set(RABBIT_MQ_TARGETS rabbitmq::rabbitmq rabbitmq::rabbitmq-static)
else ()
    set(RABBIT_MQ_TARGETS rabbitmq::rabbitmq-static)
endif ()

target_link_libraries(assfire-amqp-connector PRIVATE crossguid ${RABBIT_MQ_TARGETS})
target_include_directories(assfire-amqp-connector PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})