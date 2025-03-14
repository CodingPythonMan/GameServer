#pragma once
#include "IocpObject.h"
#include <vector>

class Session;

enum class EventType : UINT8
{
	Connect,
	Disconnect,
	Accept,
	Recv,
	Send
};

/*---------------
	IocpEvent
---------------*/

class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void			Initialize();

public:
	EventType		eventType;
	std::shared_ptr<IocpObject>	owner;
};

/*---------------
	ConnectEvent
---------------*/

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) {}
};

/*---------------
	DisconnectEvent
---------------*/

class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::Disconnect) {}
};

/*---------------
	AcceptEvent
---------------*/

class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) {}

public:
	std::shared_ptr<Session>	mSession = nullptr;
};

/*---------------
	RecvEvent
---------------*/

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) {}
};

/*---------------
	SendEvent
---------------*/

class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) {}

	std::vector<std::shared_ptr<SendBuffer>> sendBuffers;
};