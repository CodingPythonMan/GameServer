#include "GameSession.h"
#include <iostream>

GameSession::~GameSession()
{
	std::cout << "~GameSession()\n";
}

void GameSession::OnConnected()
{
}

void GameSession::OnDisconnected()
{
}

void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
}

void GameSession::OnSend(int32 len)
{
}