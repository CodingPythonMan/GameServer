#pragma once
#include <windows.h>
#include <memory>

class SendBufferChunk;

class SendBuffer
{
public:
	SendBuffer(std::shared_ptr<SendBufferChunk> owner, BYTE* buffer, UINT32 allocSize);
	~SendBuffer();

	BYTE*			Buffer() { return mBuffer; }
	UINT32			AllocSize() { return mAllocSize; }
	UINT32			WriteSize() { return mWriteSize; }
	void			Close(UINT32 writeSize);

private:
	BYTE*				mBuffer;
	UINT32				mAllocSize = 0;
	UINT32				mWriteSize = 0;
	std::shared_ptr<SendBufferChunk> mOwner;
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
	std::shared_ptr<SendBuffer>	Open(UINT32 allocSize);
	void			Close(UINT32 writeSize);

	bool			IsOpen() { return mOpen; }
	BYTE*			Buffer() { return &_buffer[_usedSize]; }
	UINT32			FreeSize() { return static_cast<UINT32>(_buffer.size()) - _usedSize; }

private:
	std::array<BYTE, SEND_BUFFER_CHUNK_SIZE>	mBuffer = {};
	bool										mOpen = false;
	UINT32										mUsedSize = 0;
};

/*-----------------------
	SendBufferManager
------------------------*/

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
	Vector<SendBufferChunkRef> _sendBufferChunks;
};