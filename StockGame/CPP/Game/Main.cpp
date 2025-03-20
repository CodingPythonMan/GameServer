#include "GameSession.h"
#include "Memory.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "Service.h"
#include "DBService.h"

#define WORKER_TICK 64

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() * WORKER_TICK;

		// ��Ʈ��ũ ����� ó�� -> �ΰ��� �������� (��Ŷ �ڵ鷯�� ����)
		service->GetIocpCore()->Dispatch(10);

		// ����� �ϰ� ó��
		ThreadManager::DistributeReservedJob();

		// �۷ι� ť
		ThreadManager::DoGlobalQueueWork();
	}
}

int main()
{
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 1537),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>,
		100);

	ASSERT_CRASH(service->Start());

	// ��!!! Poco �� �ѹ� ����غ���.
	//DBService dbService;
	//dbService.Start();

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
			{
				DoWorkerJob(service);
			});
	}

	// Main Thread
	DoWorkerJob(service);

	GThreadManager->Join();
}