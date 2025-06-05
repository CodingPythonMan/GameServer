#pragma once
#include <unordered_map>
#include "Macro.h"
#include "RefSingleton.h"
#include "Lock.h"
#include "MapData.h"

class Player;

class MapManager : public RefSingleton<MapManager>
{
public:
	void	Initialize();

	void	Update(float deltaTime);

	void	Enter(std::shared_ptr<Player> player);
	void	Leave(std::shared_ptr<Player> player);

	void	SendEnterNoti(std::shared_ptr<Player> player);

private:
	USE_LOCK;

	MapData		mMapData;

	// u_map<PlayerUID, Player>
	std::unordered_map<int64, std::shared_ptr<Player>> mPlayerList;
};