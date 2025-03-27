#include "GameSession.h"
#include "Memory.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "Service.h"
#include "DBService.h"
#include "ClientPacketHandler.h"
#include "MapManager.h"

#define WORKER_TICK 64

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() * WORKER_TICK;

		// 네트워크 입출력 처리 -> 인게임 로직까지 (패킷 핸들러에 의해)
		service->GetIocpCore()->Dispatch(10);

		// 예약된 일감 처리
		ThreadManager::DistributeReservedJob();

		// 글로벌 큐
		ThreadManager::DoGlobalQueueWork();
	}
}

int main()
{
	ClientPacketHandler::Initialize();

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 1537),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>,
		10);

	ASSERT_CRASH(service->Start());

	// 자!!! Poco 를 한번 사용해보자.
	//DBService dbService;
	//dbService.Start();

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
			{
				DoWorkerJob(service);
			});
	}

	GThreadManager->Launch([]() {
		while (true)
		{
			const uint64 startTick = ::GetTickCount64();

			// MapManager의 Update 호출
			MapManager::GetInstance().Update();

			// 20ms마다 실행하도록 간격 조정
			const uint64 endTick = ::GetTickCount64();
			const uint64 elapsed = endTick - startTick;

			if (elapsed < 20)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(20 - elapsed));
			}
		}
		});

	// Main Thread
	DoWorkerJob(service);

	GThreadManager->Join();
}