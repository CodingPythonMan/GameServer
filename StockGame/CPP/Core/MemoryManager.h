#pragma once
#include "RefSingleton.h"
#include <vector>

class MemoryPool;

class MemoryManager : RefSingleton<MemoryManager>
{
	enum
	{
		// -1024까지 32단위, -2048까지 128단위, -4096까지 256 단위
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		MAX_ALLOC_SIZE = 4096
	};

public:
	MemoryManager();
	virtual ~MemoryManager();

	void*	Allocate(int size);
	void	Release(void* ptr);

private:
	std::vector<MemoryPool*>	mPools;

	MemoryPool*					mPoolTable[MAX_ALLOC_SIZE + 1];
};