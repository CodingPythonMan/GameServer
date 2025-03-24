#include "GameSession.h"
#include <iostream>
#include "GameSessionManager.h"
#include "CoreGlobal.h"
#include "SendBuffer.h"
#include "Memory.h"
#include "ClientPacketHandler.h"

GameSession::~GameSession()
{
	std::cout << "~GameSession()\n";
}

void GameSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	// TODO : packetId 대역 체크
	ClientPacketHandler::HandlePacket(session, buffer, len);
}

void GameSession::OnSend(int32 len)
{
}