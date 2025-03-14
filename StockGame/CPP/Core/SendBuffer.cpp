#include "SendBuffer.h"
#include "Macro.h"
#include "ObjectPool.h"

SendBuffer::SendBuffer(std::shared_ptr<SendBufferChunk> owner, BYTE* buffer, UINT32 allocSize)
	: mOwner(owner), mBuffer(buffer), mAllocSize(allocSize)
{
}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::Close(UINT32 writeSize)
{
	ASSERT_CRASH(mAllocSize >= writeSize);
	mWriteSize = writeSize;
	mOwner->Close(writeSize);
}

/*-----------------------
	SendBufferChunk
------------------------*/

SendBufferChunk::SendBufferChunk()
{
}

SendBufferChunk::~SendBufferChunk()
{
}

void SendBufferChunk::Reset()
{
	mOpen = false;
	mUsedSize = 0;
}

std::shared_ptr<SendBuffer> SendBufferChunk::Open(UINT32 allocSize)
{
	ASSERT_CRASH(allocSize <= SEND_BUFFER_CHUNK_SIZE);
	ASSERT_CRASH(mOpen == false);

	if (allocSize > FreeSize())
	{
		return nullptr;
	}

	mOpen = true;

	return ObjectPool<SendBuffer>::MakeShared(std::shared_from_this(), Buffer(), allocSize);
}

void SendBufferChunk::Close(UINT32 writeSize)
{
	ASSERT_CRASH(_open == true);
	_open = false;
	_usedSize += writeSize;
}

/*-----------------------
	SendBufferManager
------------------------*/

SendBufferRef SendBufferManager::Open(uint32 size)
{
	if (LSendBufferChunk == nullptr)
	{
		LSendBufferChunk = Pop(); // WRITE_LOCK
		LSendBufferChunk->Reset();
	}

	ASSERT_CRASH(LSendBufferChunk->IsOpen() == false);

	// 다 썼으면 버리고 새거로 교체
	if (LSendBufferChunk->FreeSize() < size)
	{
		LSendBufferChunk = Pop(); // WRITE_LOCK
		LSendBufferChunk->Reset();
	}

	return LSendBufferChunk->Open(size);
}

SendBufferChunkRef SendBufferManager::Pop()
{
	{
		WRITE_LOCK;
		if (_sendBufferChunks.empty() == false)
		{
			SendBufferChunkRef sendBufferChunk = _sendBufferChunks.back();
			_sendBufferChunks.pop_back();
			return sendBufferChunk;
		}
	}

	return SendBufferChunkRef(xnew<SendBufferChunk>(), PushGlobal);
}

void SendBufferManager::Push(SendBufferChunkRef buffer)
{
	WRITE_LOCK;
	_sendBufferChunks.push_back(buffer);
}

void SendBufferManager::PushGlobal(SendBufferChunk* buffer)
{
	cout << "PushGlobal SENDBUFFERCHUNK" << endl;
	GSendBufferManager->Push(SendBufferChunkRef(buffer, PushGlobal));
}