#pragma once
#include "Types.h"
#include "Macro.h"
#include <vector>
#include <array>

class SendBufferChunk;

class SendBuffer
{
public:
	SendBuffer(SendBufferChunkRef owner, BYTE* buffer, uint32 allocSize);
	~SendBuffer();

	BYTE*			Buffer() { return mBuffer; }
	uint32			AllocSize() { return mAllocSize; }
	uint32			WriteSize() { return mWriteSize; }
	void			Close(uint32 writeSize);

private:
	BYTE*				mBuffer;
	uint32				mAllocSize = 0;
	uint32				mWriteSize = 0;
	SendBufferChunkRef	mOwner;
	
};

class SendBufferChunk : public std::enable_shared_from_this<SendBufferChunk>
{
	enum
	{
		SEND_BUFFER_CHUNK_SIZE = 6000
	};

public:
	SendBufferChunk();
	~SendBufferChunk();

	void			Reset();
	SendBufferRef	Open(uint32 allocSize);
	void			Close(uint32 writeSize);

	bool			IsOpen() { return mIsOpen; }
	BYTE*			Buffer() { return &mBuffer[mUsedSize]; }
	uint32			FreeSize() { return static_cast<uint32>(mBuffer.size()) - mUsedSize; }

private:
	std::array<BYTE, SEND_BUFFER_CHUNK_SIZE>		mBuffer = {};
	bool											mIsOpen = false;
	uint32											mUsedSize = 0;
};

class SendBufferManager
{
public:
	SendBufferRef Open(uint32 size);

private:
	SendBufferChunkRef	Pop();
	void				Push(SendBufferChunkRef buffer);

	static void			PushGlobal(SendBufferChunk* buffer);

private:
	USE_LOCK;
	std::vector<SendBufferChunkRef> mSendBufferChunks;
};