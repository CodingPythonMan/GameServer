#pragma once
#include <WS2tcpip.h>
#include <windows.h>
#include "Types.h"

enum
{
	SLIST_ALIGNMENT = 16
};

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : public SLIST_ENTRY
{
	// [MemoryHeader][Data]
	MemoryHeader(size_t size) : allocSize(size) {}

	static void* AttachHeader(MemoryHeader* header, size_t size)
	{
		new(header)MemoryHeader(size); // placement new
		return reinterpret_cast<void*>(++header);
	}

	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	size_t allocSize;
};

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	void			Push(MemoryHeader* ptr);
	MemoryHeader*	Pop();

private:
	SLIST_HEADER		_header;
	int32				_allocSize = 0;
	std::atomic<int32>	_useCount = 0;
	std::atomic<int32>	_reserveCount = 0;
};