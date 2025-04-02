#include "ServerSession.h"
#include <iostream>
#include "ServerPacketHandler.h"
#include "Service.h"

ServerSession::~ServerSession()
{
	printf("~ServerSession\n");
}

void ServerSession::OnConnected()
{
	CSEnterGameReq req;
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(req);
	GetPacketSessionRef()->Send(sendBuffer);
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