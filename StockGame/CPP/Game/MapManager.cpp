#include "MapManager.h"
#include "Player.h"
#include "ClientPacketHandler.h"
#include "GameSession.h"

void MapManager::Initialize()
{
	mMapData.LoadFromFile("map_data.json");
}

void MapManager::Update(float deltaTime)
{
	READ_LOCK;
	for (auto [uid, player] : mPlayerList)
	{
		player->Update(deltaTime);
	}
}

void MapManager::Enter(std::shared_ptr<Player> player)
{
	WRITE_LOCK;
	mPlayerList[player->mUniqueID] = player;
}

void MapManager::Leave(std::shared_ptr<Player> player)
{
	WRITE_LOCK;

	if (player != nullptr)
	{
		mPlayerList.erase(player->mUniqueID);
	}
}

void MapManager::SendEnterNoti(std::shared_ptr<Player> player)
{
	READ_LOCK;
	
	for (auto& [playerID, otherPlayer] : mPlayerList)
	{
		SCEnterGameNoti noti;
		if (player->mUniqueID == playerID)
		{
			continue;
		}

		noti.set_uniqueid(playerID);
		noti.set_direction(static_cast<int32>(otherPlayer->mDirection));
		noti.set_ismove(otherPlayer->mIsMoved);
		noti.set_x(otherPlayer->mX);
		noti.set_y(otherPlayer->mY);

		if (auto session = player->mSession.lock())
		{
			auto notiBuffer = ClientPacketHandler::MakeSendBuffer(noti);
			session->Send(notiBuffer);
		}
	}
}