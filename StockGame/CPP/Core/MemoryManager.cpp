#include "MemoryManager.h"
#include "MemoryPool.h"
#include "Macro.h"

MemoryManager::MemoryManager()
{
	int size = 0;
	int tableIndex = 0;

	for (size = 32; size <= 1024; size += 32)
	{
		MemoryPool* pool = new MemoryPool(size);
		mPools.push_back(pool);

		while (tableIndex <= size)
		{
			mPoolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	for (; size <= 2048; size += 128)
	{
		MemoryPool* pool = new MemoryPool(size);
		mPools.push_back(pool);

		while (tableIndex <= size)
		{
			mPoolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	for (; size <= 4096; size += 256)
	{
		MemoryPool* pool = new MemoryPool(size);
		mPools.push_back(pool);

		while (tableIndex <= size)
		{
			mPoolTable[tableIndex] = pool;
			tableIndex++;
		}
	}
}

MemoryManager::~MemoryManager()
{
	for (MemoryPool* pool : mPools)
	{
		delete pool;
	}

	mPools.clear();
}

void* MemoryManager::Allocate(int size)
{
	MemoryHeader* header = nullptr;
	const auto allocSize = size * sizeof(MemoryHeader);

	if (allocSize > MAX_ALLOC_SIZE)
	{
		// �޸� Ǯ�� �ִ� ũ�⸦ ����� �Ϲ� �Ҵ�
		header = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(allocSize, SLIST_ALIGNMENT));
	}
	else
	{
		// �޸� Ǯ���� �����´�.
		header = mPoolTable[allocSize]->Pop();
	}

	return MemoryHeader::AttachHeader(header, allocSize);
}

void MemoryManager::Release(void* ptr)
{
	MemoryHeader* header = MemoryHeader::DetachHeader(ptr);

	const int allocSize = header->allocSize;
	ASSERT_CRASH(allocSize > 0);

	if (allocSize > MAX_ALLOC_SIZE)
	{
		// �޸� Ǯ�� �ִ� ũ�� ����� �Ϲ� ����
		::_aligned_free(header);
	}
	else
	{
		// �޸� Ǯ�� �ݳ��Ѵ�.
		mPoolTable[allocSize]->Push(header);
	}
}