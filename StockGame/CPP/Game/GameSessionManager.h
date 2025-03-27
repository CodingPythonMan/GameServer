#pragma once
#include <set>
#include "Macro.h"

class GameSession;

using GameSessionRef = std::shared_ptr<GameSession>;

class GameSessionManager
{
public:
	GameSessionManager();

	void	Add(GameSessionRef session);
	void	Remove(GameSessionRef session);
	void	BroadCast(SendBufferRef sendBuffer, GameSessionRef excludeSession = GameSessionRef());

private:
	USE_LOCK;
	std::set<GameSessionRef> mSessionList;
};

extern GameSessionManager GSessionManager;