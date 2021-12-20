#pragma once

namespace assfire::util {
    class AmqpChannel {
    public:
        int getId() const {
            return id;
        }

    private:
        int id;
    };
}