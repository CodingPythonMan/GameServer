#include "ServerSession.h"
#include "Memory.h"
#include "Service.h"
#include "IocpCore.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

int main()
{
	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress(L"10.0.0.25", 1537),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>,
		1);

	service->Start();

	for (int32 i = 0; i < 100; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	GThreadManager->Join();
}