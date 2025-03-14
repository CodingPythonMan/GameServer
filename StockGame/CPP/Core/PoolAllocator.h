#pragma once

class PoolAllocator
{
public:
	static void*	Alloc(int size);
	static void		Release(void* ptr);
};