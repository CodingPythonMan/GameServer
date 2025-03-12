#pragma once
#include <windows.h>
#include <memory>
#include <vector>

class IocpRegistrable;
class Session;

enum class EventType : UINT8
{
	CONNECT,
	DISCONNECT,
	ACCEPT,
	RECV,
	SEND,
};

class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void		Reset();

public:
	EventType			mEventType;
	IocpRegistrable*	mOwner;
};

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::CONNECT) {}
};

class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::DISCONNECT) {}
};

class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::ACCEPT) {}

public:
	std::shared_ptr<Session>	mSession = nullptr;
};

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::RECV) {}
};

class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::SEND) {}

	//std::vector<>;
};