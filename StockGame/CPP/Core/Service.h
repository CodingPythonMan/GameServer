#pragma once
#include <functional>
#include <set>
#include "NetAddress.h"
#include "IocpCore.h"
#include "Listener.h"
#include "Macro.h"

enum class ServiceType : uint8 
{
	Server,
	Client
};


using SessionFactory = std::function<SessionRef(void)>;

class Service : public std::enable_shared_from_this<Service>
{
public:
	Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessioncount = 1);
	virtual ~Service();

	virtual bool	Start() abstract;
	bool			CanStart() { return mSessionFactory != nullptr; }

	virtual void	CloseService();
	void			SetSessionFactory(SessionFactory func) { mSessionFactory = func; }

	void			Broadcast(SendBufferRef sendBuffer);
	SessionRef		CreateSession();
	void			AddSession(SessionRef session);
	void			ReleaseSession(SessionRef session);
	int32			GetCurrentSessionCount() { return mSessionCount; }
	int32			GetMaxSessionCount() { return mMaxSessionCount; }

public:
	ServiceType		GetServiceType() { return mType; }
	NetAddress		GetNetAddress() { return mNetAddress; }
	IocpCoreRef&	GetIocpCore() { return mIocpCore; }

protected:
	USE_LOCK;

	ServiceType				mType;
	NetAddress				mNetAddress = {};
	IocpCoreRef				mIocpCore;

	std::set<SessionRef>	mSessionList;
	int32					mSessionCount = 0;
	int32					mMaxSessionCount = 0;
	SessionFactory			mSessionFactory;
};

/*--------------------
	Client Service
---------------------*/

class ClientService : public Service 
{
public:
	ClientService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~ClientService() {}

	virtual bool	Start() override;
};

/*--------------------
	Server Service
---------------------*/

class ServerService : public Service
{
public:
	ServerService(NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~ServerService() {}

	virtual bool	Start() override;
	virtual void	CloseService() override;

private:
	ListenerRef		_listener = nullptr;
};