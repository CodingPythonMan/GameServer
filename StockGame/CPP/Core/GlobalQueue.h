#pragma once
#include "Types.h"
#include "BlockingQueue.h"

class GlobalQueue
{
public:
	GlobalQueue();
	~GlobalQueue();

	void					Push(JobQueueRef jobQueue);
	JobQueueRef				Pop();

private:
	BlockingQueue<JobQueueRef>	mjobQueues;
};