#include "JobQueue.h"
#include "GlobalQueue.h"
#include "CoreTLS.h"

void JobQueue::Push(JobRef job, bool pushOnly)
{
	const int32 prevCount = mJobCount.fetch_add(1);
	mJobs.Push(job); // WRITE_LOCK

	// ù��° Job�� ���� �����尡 ������� ���
	if (prevCount == 0)
	{
		// �̹� �������� JobQueue�� ������ ����
		if (LCurrentJobQueue == nullptr && pushOnly == false)
		{
			Execute();
		}
		else
		{
			// ���� �ִ� �ٸ� �����尡 �����ϵ��� GlobalQueue �� �ѱ��.
			GGlobalQueue->Push(shared_from_this());
		}
	}
}

// 1) �ϰ��� ��-�� ������?
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

		// ���� �ϰ��� 0����� ����
		if (mJobCount.fetch_sub(jobCount) == jobCount)
		{
			LCurrentJobQueue = nullptr;
			return;
		}

		const uint64 now = ::GetTickCount64();
		if (now >= LEndTickCount)
		{
			LCurrentJobQueue = nullptr;
			// ���� �ִ� �ٸ� �����尡 �����ϵ��� GlobalQueue �� �ѱ��.
			// �ٵ� �̰� �ȴٸ�? 
			GGlobalQueue->Push(shared_from_this());
			break;
		}
	}
}