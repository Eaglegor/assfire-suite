#include "AmqpService.hpp"

namespace assfire::util {

    bool AmqpService::declareExchange(const std::string &name, const AmqpExchangeOpts &exchange_opts) {
        return false;
    }

    bool AmqpService::declareQueue(const std::string &name, const AmqpQueueOpts &queue_opts) {
        return false;
    }

    bool AmqpService::bindQueue(const AmqpQueueBinding &queue_binding) {
        return false;
    }
}