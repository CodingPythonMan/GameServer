#pragma once
#include "IocpRegistrable.h"
#include <windows.h>
#include "RingBuffer.h"
#include <atomic>

class Session : public IocpRegistrable
{
public:
	Session();
	virtual ~Session() = default;

	void	Reset();
	void	SetClientSocket(SOCKADDR_IN address);
	void	ProcessConnect();

	SOCKET				mClientSocket = INVALID_SOCKET;
	SOCKADDR_IN			mAddress = {};

	RingBuffer			mRecvBuffer;

protected:
	virtual void		OnConnected() {};

private:
	std::atomic<bool>	mIsConnected = false;

	//std::shared_ptr<>
};