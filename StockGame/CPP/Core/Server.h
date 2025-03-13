#pragma once
#include <WS2tcpip.h>
#include <windows.h>
#include <vector>
#include <memory>
#include "IocpRegistrable.h"
#include <MSWSock.h>
#include "IocpEventPool.h"

#pragma comment(lib, "WS2_32.lib")

class Service;

class Server : public IocpRegistrable
{
public:
	Server() = default;
	virtual ~Server() = default;

	void	Start(const WCHAR ip[], int port, bool nagle = false);
	void	Reset();

	// 후에 Service 스레드로 대체
	void	Update();

private:
	void	_SetBaseSocketOption(bool nagle);
	void	_Bind(const WCHAR ip[], int port);
	void	_Listen();
	void	_RegisterAccept(AcceptEvent* acceptEvent);
	void	_Accept(IocpEvent* iocpEvent, INT32 numOfBytes);

private:
	SOCKET			mListenSocket = INVALID_SOCKET;
	
	// Server 의 Main IOCP
	HANDLE			mIocp = INVALID_HANDLE_VALUE;

	std::vector<std::shared_ptr<AcceptEvent>>	mAcceptEventList;

	LPFN_ACCEPTEX	mAcceptEx = nullptr;
};