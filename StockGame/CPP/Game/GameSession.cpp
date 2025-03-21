#include "GameSession.h"
#include <iostream>
#include "GameSessionManager.h"
#include "CoreGlobal.h"
#include "SendBuffer.h"
#include "Memory.h"

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
	memcpy(recvBuffer, buffer, len+4); // 수신된 데이터 복사
	recvBuffer[len+4] = '\0'; // 문자열 종료 문자 추가

	std::cout << "수신된 데이터 (문자열 변환): " << recvBuffer << std::endl;

	// TODO : packetId 대역 체크
	//ClientPacketHandler::HandlePacket(session, buffer, len);

	SendBufferRef sendBuffer = GSendBufferManager->Open(len+4); // 동적 버퍼 생성
	memcpy(sendBuffer->Buffer(), recvBuffer, len+4); // 데이터를 버퍼에 복사
	sendBuffer->Close(len+4); // 버퍼 크기 설정

	session->Send(sendBuffer); // 클라이언트에게 응답 전송
}

void GameSession::OnSend(int32 len)
{
}