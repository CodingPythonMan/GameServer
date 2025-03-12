#pragma once
#include <WS2tcpip.h>
#include <windows.h>
#include <memory>
#include "IocpRegistrable.h"

#pragma comment(lib, "WS2_32.lib")

class Client : public IocpRegistrable
{
public:
	Client() = default;
	virtual ~Client() = default;

	void	Start(const WCHAR ip[], int port, bool nagle = false);
	void	Reset();

private:
	void	_Connect(const WCHAR ip[], int port, bool nagle);

private:
	SOCKET	mSocket = INVALID_SOCKET;
};