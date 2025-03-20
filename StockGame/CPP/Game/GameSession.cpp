#include "GameSession.h"
#include <iostream>
#include "GameSessionManager.h"
#include "CoreGlobal.h"
#include "SendBuffer.h"

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

	char recvBuffer[20] = { 0 }; // +1: 널 종료 문자 추가
	memcpy(recvBuffer, buffer, len); // 수신된 데이터 복사
	recvBuffer[len+1] = '\0'; // 문자열 종료 문자 추가

	std::cout << "수신된 데이터 (문자열 변환): " << recvBuffer << std::endl;

	// TODO : packetId 대역 체크
	// 여기가 이제 컨텐츠 들어가는 단인데... 천천히 만들어보자.
	//ClientPacketHandler::HandlePacket(session, buffer, len);

	SendBufferRef sendBuffer = GSendBufferManager->Open(len - 4);
	session->Send(sendBuffer);
	
}

void GameSession::OnSend(int32 len)
{
}