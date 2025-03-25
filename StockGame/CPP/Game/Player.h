#pragma once
#include "GameSession.h"
#include "Macro.h"
#include <memory>

class Player
{
public:
	Player();
	virtual ~Player();

	int64			mUniqueID = 0;
	std::string		mName;
	
	// Position
	int				mX = 0;
	int				mY = 0;

	// 순환 참조 될 수 있음
	std::shared_ptr<class GameSession>	mSession;
};