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

void Session::SetClientSocket(SOCKADDR_IN address)
{
	mAddress = address;
}

void Session::ProcessConnect()
{
	mIsConnected.store(true);

	// Session 수를 관리하는 곳이 어디인가?


	OnConnected();
}