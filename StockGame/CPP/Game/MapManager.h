#pragma once
#include <unordered_map>
#include "Macro.h"
#include "RefSingleton.h"
#include "Lock.h"

class Player;

class MapManager : public RefSingleton<MapManager>
{
public:
	void	Update();

	void	Enter(std::shared_ptr<Player> player);
	void	Leave(std::shared_ptr<Player> player);
	
public:
	USE_LOCK;

	// u_map<PlayerUID, Player>
	std::unordered_map<int64, std::shared_ptr<Player>> mPlayerList;
};