#pragma once

namespace assfire::util
{
    template<typename T>
    struct AmqpProtoParse
    {
        T operator()(void *bytes, int len) const {
            T msg;
            msg.ParseFromArray(bytes, len);
            return msg;
        }
    };
}