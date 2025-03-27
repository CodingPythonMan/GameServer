#include "GameSession.h"
#include "GameSessionManager.h"
#include "Player.h"

GameSessionManager GSessionManager;

GameSessionManager::GameSessionManager()
{
	
}

void GameSessionManager::Add(GameSessionRef session)
{
	WRITE_LOCK;
	mSessionList.insert(session);
}

void GameSessionManager::Remove(GameSessionRef session)
{
	WRITE_LOCK;
	mSessionList.erase(session);
}

void GameSessionManager::BroadCast(SendBufferRef sendBuffer, GameSessionRef excludeSession)
{
	WRITE_LOCK;
	for (GameSessionRef session : mSessionList)
	{
		if (session == excludeSession)
		{
			continue;
		}

		session->Send(sendBuffer);
	}
}
