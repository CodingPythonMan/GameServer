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

	char recvBuffer[20] = { 0 }; // +1: �� ���� ���� �߰�
	memcpy(recvBuffer, buffer, len); // ���ŵ� ������ ����
	recvBuffer[len+1] = '\0'; // ���ڿ� ���� ���� �߰�

	std::cout << "���ŵ� ������ (���ڿ� ��ȯ): " << recvBuffer << std::endl;

	// TODO : packetId �뿪 üũ
	// ���Ⱑ ���� ������ ���� ���ε�... õõ�� ������.
	//ClientPacketHandler::HandlePacket(session, buffer, len);

	SendBufferRef sendBuffer = GSendBufferManager->Open(len - 4);
	session->Send(sendBuffer);
	
}

void GameSession::OnSend(int32 len)
{
}