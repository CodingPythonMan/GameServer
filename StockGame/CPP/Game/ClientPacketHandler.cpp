#include "ClientPacketHandler.h"
#include "Player.h"
#include "Memory.h"
#include "GameSessionManager.h"
#include "MapManager.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

// 직접 컨텐츠 작업자

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
	// 우선 맵은 1개인 상태

	// ID 발급 (DB 아이디가 아니고, 인게임 아이디)
	static std::atomic<int64> idGenerator = 1;

	std::shared_ptr<Player> player = MakeShared<Player>();
	player->mUniqueID = idGenerator++;
	// Player 이름 대야한다.
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

	return true;
}

bool OnCSMoveReq(PacketSessionRef& session, CSMoveReq& pkt)
{
	std::shared_ptr<GameSession> gameSession = std::static_pointer_cast<GameSession>(session);

	auto player = gameSession->mPlayer;

	SCMoveNoti noti;
	noti.set_uniqueid(player->mUniqueID);
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

	SCStopNoti noti;
	noti.set_uniqueid(player->mUniqueID);
	noti.set_x(player->mX);
	noti.set_y(player->mY);

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(noti);
	GSessionManager.BroadCast(sendBuffer);

	return true;
}