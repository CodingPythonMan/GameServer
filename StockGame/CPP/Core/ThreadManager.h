#pragma once
#include <thread>
#include <functional>
#include <mutex>

/*------------------
	ThreadManager
-------------------*/

class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	void	Launch(std::function<void(void)> callback);
	void	Join();

	static void InitTLS();
	static void DestroyTLS();

	static void DoGlobalQueueWork();
	static void DistributeReservedJob();

private:
	std::mutex			_lock;
	std::vector<std::thread>	_threads;
};