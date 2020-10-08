#pragma once

#include <WinSock2.h>
#include <exception>
#include <string>

namespace assfire {
    namespace win32 {
        struct winsock_initializer
        {
            struct winsock_initialization_exception : public std::exception
            {
                winsock_initialization_exception(int code) :
                        message(std::string("Winsock initialization failed: error ") + std::to_string(code))
                {}

                const char *what() const override
                {
                    return message.c_str();
                }

                std::string message;
            };

            winsock_initializer()
            {
                WORD version = MAKEWORD(2, 2);
                WSADATA data;
                if (WSAStartup(version, &data) != 0) {
                    throw winsock_initialization_exception(WSAGetLastError());
                }
            }

            ~winsock_initializer()
            {
                WSACleanup();
            }
        };
    }
}