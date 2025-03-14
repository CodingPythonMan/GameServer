#pragma once
#include <windows.h>
#include "MemoryPool.h"

template<typename Type>
class ObjectPool
{
public:
	template<typename... Args>
	static Type* Pop(Args&&... args)
	{
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(s_pool.Pop(), s_allocSize));

		new(memory)Type(std::forward<Args>(args)...);// placement new
		return memory;
	}

	static void Push(Type* obj)
	{
		obj->~Type();
		s_pool.Push(MemoryHeader::DetachHeader(obj));
	}


	template<typename... Args>
	static shared_ptr<Type> MakeShared(Args&&... args)
	{
		shared_ptr<Type> ptr = { Pop(std::forward<Args>(args)...), Push };
		return ptr;
	}

private:
	static INT32			s_allocSize;
	static MemoryPool		s_pool;
};

template<typename Type>
INT32 ObjectPool<Type>::sAllocSize = sizeof(Type) * sizeof(MemoryHeader);

template<typename Type>
MemoryPool ObjectPool<Type>::sPool{ s_allocSize };