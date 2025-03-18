#include "GlobalQueue.h"

GlobalQueue::GlobalQueue()
{
}

GlobalQueue::~GlobalQueue()
{
}

void GlobalQueue::Push(JobQueueRef jobQueue)
{
	mjobQueues.Push(jobQueue);
}

JobQueueRef GlobalQueue::Pop()
{
	return mjobQueues.Pop();
}
