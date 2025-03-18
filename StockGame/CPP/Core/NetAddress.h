#pragma once
#include <WS2tcpip.h>
#include <windows.h>
#include "Types.h"

#pragma comment(lib, "ws2_32.lib")

class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sockAddr);
	NetAddress(std::wstring ip, uint16 port);

	SOCKADDR_IN&		GetSockAddr() { return _sockAddr; }
	std::wstring			GetIpAddress();
	uint16			GetPort() { return ::ntohs(_sockAddr.sin_port); }

public:
	static IN_ADDR Ip2Address(const WCHAR* ip);

private:
	SOCKADDR_IN _sockAddr = {};
};

