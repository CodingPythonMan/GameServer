#include "Service.h"
#include "Session.h"
#include "Listener.h"
#include "Memory.h"

Service::Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessioncount)
	:mType(type), mNetAddress(address), mIocpCore(core), mSessionFactory(factory), mMaxSessionCount(maxSessioncount)
{

}

Service::~Service()
{
}

void Service::CloseService()
{
}

void Service::Broadcast(SendBufferRef sendBuffer)
{
	WRITE_LOCK;
	for (const auto& session : mSessionList)
	{
		session->Send(sendBuffer);
	}
}

SessionRef Service::CreateSession()
{
	SessionRef session = mSessionFactory();
	session->SetService(shared_from_this());

	if (false == mIocpCore->Register(session))
	{
		return nullptr;
	}

	return session;
}

void Service::AddSession(SessionRef session)
{
	WRITE_LOCK;
	mSessionCount++;
	mSessionList.insert(session);
}

void Service::ReleaseSession(SessionRef session)
{
	WRITE_LOCK;
	ASSERT_CRASH(mSessionList.erase(session) != 0);
	mSessionCount--;
}

/*--------------------
	Client Service
---------------------*/

ClientService::ClientService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
	: Service(ServiceType::Client, targetAddress, core, factory, maxSessionCount)
{
}

bool ClientService::Start()
{
	if (CanStart() == false)
	{
		return false;
	}

	// 한번에 모두 들어가는게 아닌 텀을 두고 연결한다.
	std::thread([=]() {
		const int32 sessionCount = GetMaxSessionCount();
		for (int32 i = 0; i < sessionCount; i++)
		{
			SessionRef session = CreateSession();
			if (false == session->Connect())
			{
				return false;
			}

			Sleep(100);
		}
		return true;
		}).detach();

	return true;
}

ServerService::ServerService(NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
	: Service(ServiceType::Server, address, core, factory, maxSessionCount)
{
}

bool ServerService::Start()
{
	if (CanStart() == false)
		return false;

	_listener = MakeShared<Listener>();
	if (_listener == nullptr)
		return false;

	ServerServiceRef service = static_pointer_cast<ServerService>(shared_from_this());
	if (_listener->StartAccept(service) == false)
		return false;

	return true;
}

void ServerService::CloseService()
{
	Service::CloseService();
}
