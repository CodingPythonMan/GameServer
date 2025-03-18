#pragma once
#include "Types.h"
#include "MemoryPool.h"

template<typename Type>
class ObjectPool
{
public:
	template<typename... Args>
	static Type* Pop(Args&&... args)
	{
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(mPool.Pop(), mAllocSize));
		
		new(memory)Type(std::forward<Args>(args)...);// placement new
		return memory;
	}

	static void Push(Type* obj)
	{
		obj->~Type();
		mPool.Push(MemoryHeader::DetachHeader(obj));
	}

	template<typename... Args>
	static std::shared_ptr<Type> MakeShared(Args&&... args)
	{
		std::shared_ptr<Type> ptr = { Pop(std::forward<Args>(args)...), Push };
		return ptr;
	}

private:
	static int32			mAllocSize;
	static MemoryPool		mPool;
};

template<typename Type>
int32 ObjectPool<Type>::mAllocSize = sizeof(Type) * sizeof(MemoryHeader);

template<typename Type>
MemoryPool ObjectPool<Type>::mPool{ mAllocSize };