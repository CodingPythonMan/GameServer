#include "PoolAllocator.h"
#include "MemoryManager.h"

void* PoolAllocator::Alloc(int size)
{
	return MemoryManager::GetInstance().Allocate(size);
}

void PoolAllocator::Release(void* ptr)
{
	MemoryManager::GetInstance.Release(ptr);
}