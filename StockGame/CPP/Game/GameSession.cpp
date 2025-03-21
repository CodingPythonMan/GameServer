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

	char recvBuffer[20] = { 0 }; // +1: �� ���� ���� �߰�
	memcpy(recvBuffer, buffer, len+4); // ���ŵ� ������ ����
	recvBuffer[len+4] = '\0'; // ���ڿ� ���� ���� �߰�

	std::cout << "���ŵ� ������ (���ڿ� ��ȯ): " << recvBuffer << std::endl;

	// TODO : packetId �뿪 üũ
	//ClientPacketHandler::HandlePacket(session, buffer, len);

	SendBufferRef sendBuffer = GSendBufferManager->Open(len+4); // ���� ���� ����
	memcpy(sendBuffer->Buffer(), recvBuffer, len+4); // �����͸� ���ۿ� ����
	sendBuffer->Close(len+4); // ���� ũ�� ����

	session->Send(sendBuffer); // Ŭ���̾�Ʈ���� ���� ����
}

void GameSession::OnSend(int32 len)
{
}