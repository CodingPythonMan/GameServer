#include <WS2tcpip.h>
#include "Session.h"

Session::Session()
{
	Reset();

	mClientSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

void Session::Reset()
{
}