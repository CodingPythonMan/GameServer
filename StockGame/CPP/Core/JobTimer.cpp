#include "JobTimer.h"
#include "JobQueue.h"

void JobTimer::Reserve(uint64 tickAfter, std::weak_ptr<JobQueue> owner, JobRef job)
{
	const uint64 executeTick = ::GetTickCount64() + tickAfter;
	JobData* jobData = ObjectPool<JobData>::Pop(owner, job);

	WRITE_LOCK;

	mItemList.push(TimerItem{ executeTick, jobData });
}

void JobTimer::Distribute(uint64 now)
{
	// �ѹ��� 1 �����常 ���
	if (mIsDistributing.exchange(true) == true)
		return;

	std::vector<TimerItem> items;

	{
		WRITE_LOCK;

		while (mItemList.empty() == false)
		{
			const TimerItem& timerItem = mItemList.top();
			if (now < timerItem.mExecuteTick)
				break;

			items.push_back(timerItem);
			mItemList.pop();
		}
	}

	for (TimerItem& item : items)
	{
		if (JobQueueRef owner = item.mJobData->mOwner.lock())
			owner->Push(item.mJobData->mJob);

		ObjectPool<JobData>::Push(item.mJobData);
	}

	// �������� �ٿ��ش�.
	mIsDistributing.store(false);
}

void JobTimer::Clear()
{
	WRITE_LOCK;

	while (mItemList.empty() == false)
	{
		const TimerItem& timerItem = mItemList.top();
		ObjectPool<JobData>::Push(timerItem.mJobData);
		mItemList.pop();
	}
}