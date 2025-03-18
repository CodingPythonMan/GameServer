#pragma once
#include <memory>
#include <queue>
#include "Types.h"
#include "Macro.h"

class JobQueue;

struct JobData
{
	JobData(std::weak_ptr<JobQueue> owner, JobRef job) : mOwner(owner), mJob(job)
	{
	}

	std::weak_ptr<JobQueue>	mOwner;
	JobRef					mJob;
};

struct TimerItem
{
	bool operator<(const TimerItem& other) const
	{
		return mExecuteTick > other.mExecuteTick;
	}

	uint64 mExecuteTick = 0;
	JobData* mJobData = nullptr;
};

class JobTimer
{
public:
	void	Reserve(uint64 tickAfter, std::weak_ptr<JobQueue> owner, JobRef job);
	void	Distribute(uint64 now);
	void	Clear();

private:
	USE_LOCK;
	std::priority_queue<TimerItem>	mItemList;
	std::atomic<bool>				mIsDistributing = false;
};

