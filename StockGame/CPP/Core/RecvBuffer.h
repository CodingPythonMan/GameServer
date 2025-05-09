#pragma once
#include "Types.h"
#include <vector>

class RecvBuffer
{
	enum {BUFFER_COUNT = 10};
public:
	RecvBuffer(int32 bufferSize);
	~RecvBuffer();

	void			Clean();
	bool			OnRead(int32 numOfBytes);
	bool			OnWrite(int32 numOfBytes);

	BYTE*			GetReadPos() { return &mBuffer[mReadPos]; }
	BYTE*			GetWritePos() { return &mBuffer[mWritePos]; }
	int32			GetDataSize() { return mWritePos - mReadPos; }
	int32			GetFreeSize() { return mCapacity - mWritePos; }

private:
	int32				mCapacity = 0;
	int32				mBufferSize = 0;
	int32				mReadPos = 0;
	int32				mWritePos = 0;
	std::vector<BYTE>	mBuffer;
};