#include "SendBuffer.h"
#include "ObjectPool.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"
#include "Memory.h"
#include "ConsoleLog.h"

SendBuffer::SendBuffer(SendBufferChunkRef owner, BYTE* buffer, uint32 allocSize)
	: mOwner(owner), mBuffer(buffer), mAllocSize(allocSize)
{

}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::Close(uint32 writeSize)
{
	ASSERT_CRASH(mAllocSize >= writeSize);
	mWriteSize = writeSize;
	mOwner->Close(writeSize);
}

SendBufferChunk::SendBufferChunk()
{
}

SendBufferChunk::~SendBufferChunk()
{
}

void SendBufferChunk::Reset()
{
	mIsOpen = false;
	mUsedSize = 0;
}

SendBufferRef SendBufferChunk::Open(uint32 allocSize)
{
	ASSERT_CRASH(allocSize <= SEND_BUFFER_CHUNK_SIZE);
	ASSERT_CRASH(mIsOpen == false);

	if (allocSize > FreeSize())
		return nullptr;

	mIsOpen = true;

	return ObjectPool<SendBuffer>::MakeShared(shared_from_this(), Buffer(), allocSize);
}

void SendBufferChunk::Close(uint32 writeSize)
{
	ASSERT_CRASH(mIsOpen == true);
	mIsOpen = false;
	mUsedSize += writeSize;
}

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
		if (mSendBufferChunks.empty() == false)
		{
			SendBufferChunkRef sendBufferChunk = mSendBufferChunks.back();
			mSendBufferChunks.pop_back();
			return sendBufferChunk;
		}
	}

	return SendBufferChunkRef(xnew<SendBufferChunk>(), PushGlobal);
}

void SendBufferManager::Push(SendBufferChunkRef buffer)
{
	WRITE_LOCK;
	mSendBufferChunks.push_back(buffer);
}

void SendBufferManager::PushGlobal(SendBufferChunk* buffer)
{
	//GConsoleLogger->WriteStdOut(Color::GREEN, L"PushGlobal SENDBUFFERCHUNK\n");
	GSendBufferManager->Push(SendBufferChunkRef(buffer, PushGlobal));
}