#include "Player.h"

Player::Player()
{
	mX = rand() % 100;
	mY = rand() % 100;
}

Player::~Player()
{
	mUniqueID = 0;
	mX = 0;
	mY = 0;
	mIsMoved = false;
}