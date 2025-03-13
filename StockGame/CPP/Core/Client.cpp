#include "Client.h"
#include "Macro.h"

void Client::Start(const WCHAR ip[], int port, bool nagle)
{
	// init
	WSADATA wsa;
	ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), &wsa) == 0);

	mSocket = socket(AF_INET, SOCK_STREAM, 0);
	ASSERT_CRASH(mSocket != INVALID_SOCKET);

	// connect
	_Connect(ip, port, nagle);
}

void Client::Reset()
{
}

void Client::_Connect(const WCHAR ip[], int port, bool nagle)
{
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::InetPton(AF_INET, ip, &serverAddr.sin_addr);
	serverAddr.sin_port = htons(port);
	int retval = connect(mSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	ASSERT_CRASH(retval != SOCKET_ERROR);
}