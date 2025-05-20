#pragma once
#include "Types.h"
#include <stack>

struct MonitorData;

extern thread_local uint32				LThreadId;
extern thread_local uint64				LEndTickCount;

extern thread_local std::stack<int32>	LLockStack;
extern thread_local SendBufferChunkRef	LSendBufferChunk;
extern thread_local class JobQueue*		LCurrentJobQueue;

// 모니터링 용
extern thread_local MonitorData*		LMonitorData;