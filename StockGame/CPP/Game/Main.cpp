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
		NetAddress(L"10.0.0.25", 1537),
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
		uint64 lastTick = ::GetTickCount64();

		while (true)
		{
			uint64 now = ::GetTickCount64();
			uint64 delta = now - lastTick;
			lastTick = now;

			float deltaTime = delta / 1000.0f;  // 초 단위 변환 (예: 0.02)

			MapManager::GetInstance().Update(deltaTime);

			const uint64 endTick = ::GetTickCount64();
			const uint64 elapsed = endTick - now;

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