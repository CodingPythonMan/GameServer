#include "MapManager.h"
#include "Player.h"

void MapManager::Update()
{
	READ_LOCK;
	for (auto [uid, player] : mPlayerList)
	{
		player->Update();
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
	mPlayerList.erase(player->mUniqueID);
}