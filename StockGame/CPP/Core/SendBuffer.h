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

	BYTE*			Buffer() { return _buffer; }
	uint32			AllocSize() { return _allocSize; }
	uint32			WriteSize() { return _writeSize; }
	void			Close(uint32 writeSize);

private:
	BYTE*				_buffer;
	uint32				_allocSize = 0;
	uint32				_writeSize = 0;
	SendBufferChunkRef	_owner;
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

	bool			IsOpen() { return _open; }
	BYTE*			Buffer() { return &mBuffer[_usedSize]; }
	uint32			FreeSize() { return static_cast<uint32>(mBuffer.size()) - _usedSize; }

private:
	std::array<BYTE, SEND_BUFFER_CHUNK_SIZE>		mBuffer = {};
	bool									_open = false;
	uint32									_usedSize = 0;
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
	std::vector<SendBufferChunkRef> _sendBufferChunks;
};