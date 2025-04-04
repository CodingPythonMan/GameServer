#pragma once
#include "GameEnum.h"
#include <memory>
#include "Macro.h"

class GameSession;

class Player
{
public:
	Player(std::weak_ptr<GameSession> session);
	virtual ~Player();

	void			Update(float deltaTime);

public:
	int64			mUniqueID = 0;
	std::string		mName;
	
	// Position
	float			mX = 0;
	float			mY = 0;
	bool			mIsMoved = false;
	EDirection		mDirection = EDirection::LL;

	float			mSpeed = 200.0f;

	std::weak_ptr<GameSession>	mSession;
};