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

    // �̵� ���� ����ȭ (�밢�� �̵� �ÿ��� ������ �̵����� �ǵ���)
    float length = static_cast<float>(sqrt(moveX * moveX + moveY * moveY));
    if (length != 0.0f)
    {
        moveX /= length;
        moveY /= length;
    }

    // ���� mSpeed(Ȥ�� tick �� �̵���)�� ���ؼ� �̵���Ű�� �ʹٸ� �߰�
    mX += moveX; // �Ǵ� mX += moveX * mSpeed;
    mY += moveY; // �Ǵ� mY += moveY * mSpeed;
}