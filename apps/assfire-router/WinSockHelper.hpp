#pragma once

#include <WinSock2.h>
#include <spdlog/spdlog.h>

namespace assfire
{
	struct WinSockHelper
	{
		WinSockHelper() {
			WORD version = MAKEWORD(2, 2);
			WSADATA data;
			if (WSAStartup(version, &data) != 0) {
				SPDLOG_ERROR("Winsock initialization failed: error {}", WSAGetLastError());
			}
		}

		~WinSockHelper() {
			WSACleanup();
		}
	};
}