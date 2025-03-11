#pragma once
#include <WS2tcpip.h>
#include <windows.h>
#include <vector>
#include <memory>
#include "IocpRegistrable.h"

#pragma comment(lib, "WS2_32.lib")

class Service;
class AcceptEvent;

class Server : public IocpRegistrable
{
public:
	Server() = default;
	virtual ~Server() = default;

	void	Start(const WCHAR ip[], int port, bool nagle = false);
	void	Clear();

private:
	void	_SetBaseSocketOption(bool nagle);
	void	_Bind(const WCHAR ip[], int port);
	void	_Listen();
	void	_RegisterAccept(std::shared_ptr<AcceptEvent> acceptEvent);

private:
	SOCKET		mListenSocket = INVALID_SOCKET;
	
	// Server ÀÇ Main IOCP
	HANDLE		mIocp = INVALID_HANDLE_VALUE;

	std::vector<std::shared_ptr<AcceptEvent>>	mAcceptEventList;
};