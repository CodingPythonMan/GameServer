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
	// Connect 가 되면, 그 때 객체를 생성해서 객체가 움직이게 하면 되긴한다.
	// C++ Client 는 봇이라고 봐야한다.
	std::thread([=]() {
		{
			CSEnterGameReq req;
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(req);
			GetPacketSessionRef()->Send(sendBuffer);
		}

		while (true)
		{
			Sleep(3000);

			{
				CSMoveReq req;
				req.set_direction(static_cast<int32>(EDirection::RR));
				auto sendBuffer = ServerPacketHandler::MakeSendBuffer(req);
				GetPacketSessionRef()->Send(sendBuffer);
			}

			Sleep(1000);

			{
				CSMoveReq req;
				req.set_direction(static_cast<int32>(EDirection::DD));
				auto sendBuffer = ServerPacketHandler::MakeSendBuffer(req);
				GetPacketSessionRef()->Send(sendBuffer);
			}

			Sleep(1000);

			{
				CSMoveReq req;
				req.set_direction(static_cast<int32>(EDirection::LL));
				auto sendBuffer = ServerPacketHandler::MakeSendBuffer(req);
				GetPacketSessionRef()->Send(sendBuffer);
			}

			Sleep(1000);

			{
				CSMoveReq req;
				req.set_direction(static_cast<int32>(EDirection::UU));
				auto sendBuffer = ServerPacketHandler::MakeSendBuffer(req);
				GetPacketSessionRef()->Send(sendBuffer);
			}

			Sleep(1000);

			{
				CSStopReq req;
				auto sendBuffer = ServerPacketHandler::MakeSendBuffer(req);
				GetPacketSessionRef()->Send(sendBuffer);
			}
		}
		}).detach();
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