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

	// Session ���� �����ϴ� ���� ����ΰ�?


	OnConnected();
}