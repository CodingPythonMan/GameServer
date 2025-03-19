#pragma once
#include <set>
#include "Macro.h"

class GameSession;

using GameSessionRef = std::shared_ptr<GameSession>;

class GameSessionManager
{
public:
	void Add(GameSessionRef session);
	void Remove(GameSessionRef session);
	void BroadCast(SendBufferRef sendBuffer);

private:
	USE_LOCK;
	std::set<GameSessionRef> _sessions;
};

extern GameSessionManager GSessionManager;