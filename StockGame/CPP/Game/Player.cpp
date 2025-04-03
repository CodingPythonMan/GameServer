#include "Player.h"

Player::Player(std::weak_ptr<GameSession> session)
{
    mX = static_cast<float>(rand() % 100);
    mY = static_cast<float>(rand() % 100);
    
    mSession = session;
}

Player::~Player()
{
	mUniqueID = 0;
	mX = 0;
	mY = 0;
	mIsMoved = false;
    mDirection = EDirection::LL;
}

void Player::Update()
{
	if (mIsMoved == false)
	{
		return;
	}

    float moveX = 0.0f;
    float moveY = 0.0f;

    switch (mDirection)
    {
    case EDirection::LL:
        moveX = -1.0f;
        break;
    case EDirection::LU:
        moveX = -1.0f;
        moveY = -1.0f;
        break;
    case EDirection::UU:
        moveY = -1.0f;
        break;
    case EDirection::RU:
        moveX = 1.0f;
        moveY = -1.0f;
        break;
    case EDirection::RR:
        moveX = 1.0f;
        break;
    case EDirection::RD:
        moveX = 1.0f;
        moveY = 1.0f;
        break;
    case EDirection::DD:
        moveY = 1.0f;
        break;
    case EDirection::LD:
        moveX = -1.0f;
        moveY = 1.0f;
        break;
    default:
        break;
    }

    // 이동 벡터 정규화 (대각선 이동 시에도 동일한 이동량이 되도록)
    float length = static_cast<float>(sqrt(moveX * moveX + moveY * moveY));
    if (length != 0.0f)
    {
        moveX /= length;
        moveY /= length;
    }

    // 만약 mSpeed(혹은 tick 당 이동량)를 곱해서 이동시키고 싶다면 추가
    mX += moveX; // 또는 mX += moveX * mSpeed;
    mY += moveY; // 또는 mY += moveY * mSpeed;
}