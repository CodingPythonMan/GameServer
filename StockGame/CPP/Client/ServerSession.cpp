#include "ServerSession.h"
#include <iostream>
#include "ServerPacketHandler.h"
#include "Service.h"
#include "GameEnum.h"

ServerSession::~ServerSession()
{
	printf("~ServerSession\n");
}

void ServerSession::OnConnected()
{
	{
		CSEnterGameReq req;
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(req);
		GetPacketSessionRef()->Send(sendBuffer);
	}

	Sleep(100);

	{
		CSMoveReq req;
		req.set_direction(static_cast<int32>(EDirection::RR));
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(req);
		GetPacketSessionRef()->Send(sendBuffer);
	}

	Sleep(100);

	{
		CSMoveReq req;
		req.set_direction(static_cast<int32>(EDirection::DD));
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(req);
		GetPacketSessionRef()->Send(sendBuffer);
	}

	Sleep(100);

	{
		CSMoveReq req;
		req.set_direction(static_cast<int32>(EDirection::LL));
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(req);
		GetPacketSessionRef()->Send(sendBuffer);
	}

	Sleep(100);

	{
		CSMoveReq req;
		req.set_direction(static_cast<int32>(EDirection::UU));
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(req);
		GetPacketSessionRef()->Send(sendBuffer);
	}

	Sleep(100);

	{
		CSStopReq req;
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(req);
		GetPacketSessionRef()->Send(sendBuffer);
	}
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