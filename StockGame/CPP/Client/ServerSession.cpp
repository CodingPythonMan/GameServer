#include "ServerSession.h"
#include <iostream>

ServerSession::~ServerSession()
{
	printf("~ServerSession\n");
}

void ServerSession::OnConnected()
{
}

void ServerSession::OnRecvPacket(BYTE* buffer, int32 len)
{
}

void ServerSession::OnSend(int32 len)
{
}

void ServerSession::OnDisconnected()
{
}