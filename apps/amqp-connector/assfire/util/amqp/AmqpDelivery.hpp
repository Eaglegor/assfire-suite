#pragma once

#include <functional>

namespace assfire::util
{
    class AmqpDelivery
    {
    public:
        template<typename T>
        using MessageConsumer = std::function<T(void *, int)>;

        enum class AckStatus
        {
            NONE,
            ACK,
            NACK
        };

        AmqpDelivery(void *data, int len)
                : data(data), len(len) {}

        template<typename T>
        T parse(MessageConsumer<T> parseFunc) {
            return parseFunc(data, len);
        }

        void ack() {
            ack_status = AckStatus::ACK;
        };

        void nack(bool requeue) {
            ack_status = AckStatus::NACK;
            should_requeue = requeue;
        };

        AckStatus getAckStatus() {
            return ack_status;
        }

        bool shouldRequeue() {
            return should_requeue;
        }

        int intShouldRequeue() {
            return shouldRequeue() ? 1 : 0;
        }

    private:
        void *data;
        int len;
        bool should_requeue;
        AckStatus ack_status = AckStatus::NONE;
    };
}