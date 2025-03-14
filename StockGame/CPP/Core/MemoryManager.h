#pragma once
#include "RefSingleton.h"
#include <vector>

class MemoryPool;

class MemoryManager : RefSingleton<MemoryManager>
{
	enum
	{
		// -1024���� 32����, -2048���� 128����, -4096���� 256 ����
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