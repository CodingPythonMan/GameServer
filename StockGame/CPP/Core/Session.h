#pragma once
#include "IocpRegistrable.h"
#include <windows.h>
#include "RingBuffer.h"

class Session : public IocpRegistrable
{
public:
	Session();
	virtual ~Session() = default;

	void	Reset();

	SOCKET			mClientSocket = INVALID_SOCKET;

	RingBuffer		mRecvBuffer;
};