#include "JobQueue.h"
#include "GlobalQueue.h"
#include "CoreTLS.h"

void JobQueue::Push(JobRef job, bool pushOnly)
{
	const int32 prevCount = mJobCount.fetch_add(1);
	mJobs.Push(job); // WRITE_LOCK

	// 첫번째 Job을 넣은 쓰레드가 실행까지 담당
	if (prevCount == 0)
	{
		// 이미 실행중인 JobQueue가 없으면 실행
		if (LCurrentJobQueue == nullptr && pushOnly == false)
		{
			Execute();
		}
		else
		{
			// 여유 있는 다른 쓰레드가 실행하도록 GlobalQueue 에 넘긴다.
			GGlobalQueue->Push(shared_from_this());
		}
	}
}

// 1) 일감이 너-무 몰리면?
void JobQueue::Execute()
{
	LCurrentJobQueue = this;

	while (true)
	{
		std::vector<JobRef> jobs;
		mJobs.PopAll(OUT jobs);

		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; i++)
		{
			jobs[i]->Execute();
		}

		// 남은 일감이 0개라면 종료
		if (mJobCount.fetch_sub(jobCount) == jobCount)
		{
			LCurrentJobQueue = nullptr;
			return;
		}

		const uint64 now = ::GetTickCount64();
		if (now >= LEndTickCount)
		{
			LCurrentJobQueue = nullptr;
			// 여유 있는 다른 쓰레드가 실행하도록 GlobalQueue 에 넘긴다.
			// 근데 이게 싫다면? 
			GGlobalQueue->Push(shared_from_this());
			break;
		}
	}
}