#include "ClientPacketHandler.h"
#include "Player.h"
#include "Memory.h"
#include "GameSessionManager.h"
#include "MapManager.h"
#include "GameSession.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

// ���� ������ �۾���

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	return false;
}

bool OnCSEchoReq(PacketSessionRef& session, CSEchoReq& req)
{
	SCEchoAck ack;
	ack.set_text(req.text());

	printf("%s\n", ack.text().c_str());

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(ack);
	session->Send(sendBuffer);

	return true;
}

bool OnCSEnterGameReq(PacketSessionRef& session, CSEnterGameReq& pkt)
{
	std::shared_ptr<GameSession> gameSession = std::static_pointer_cast<GameSession>(session);
	// �켱 ���� 1���� ����

	// ID �߱� (DB ���̵� �ƴϰ�, �ΰ��� ���̵�)
	static std::atomic<int64> idGenerator = 1;

	std::shared_ptr<Player> player = MakeShared<Player>(gameSession);
	player->mUniqueID = idGenerator++;
	// Player �̸� ����Ѵ�.
	//player->mName = 
	gameSession->mPlayer = player;

	MapManager::GetInstance().Enter(player);

	SCEnterGameAck ack;
	ack.set_x(player->mX);
	ack.set_y(player->mY);

	auto ackBuffer = ClientPacketHandler::MakeSendBuffer(ack);
	gameSession->Send(ackBuffer);

	SCEnterGameNoti noti;
	noti.set_uniqueid(player->mUniqueID);
	noti.set_x(player->mX);
	noti.set_y(player->mY);

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(noti);
	GSessionManager.BroadCast(sendBuffer, gameSession);

	// �ֺ��� �ִ� ������ ��Ŷ�� ��Ƽ� ��������Ѵ�.
	MapManager::GetInstance().SendEnterNoti(player);

	return true;
}

bool OnCSMoveReq(PacketSessionRef& session, CSMoveReq& pkt)
{
	std::shared_ptr<GameSession> gameSession = std::static_pointer_cast<GameSession>(session);

	auto player = gameSession->mPlayer;

	// ���� �÷��̾�� �����̴� ��.
	player->mIsMoved = true;

	SCMoveNoti noti;
	noti.set_uniqueid(player->mUniqueID);
	noti.set_direction(static_cast<int32>(player->mDirection));
	noti.set_x(player->mX);
	noti.set_y(player->mY);

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(noti);
	GSessionManager.BroadCast(sendBuffer);

	return true;
}

bool OnCSStopReq(PacketSessionRef& session, CSStopReq& pkt)
{
	std::shared_ptr<GameSession> gameSession = std::static_pointer_cast<GameSession>(session);

	auto player = gameSession->mPlayer;
	player->mIsMoved = false;

	SCStopNoti noti;
	noti.set_uniqueid(player->mUniqueID);
	noti.set_x(player->mX);
	noti.set_y(player->mY);

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(noti);
	GSessionManager.BroadCast(sendBuffer);

	return true;
}