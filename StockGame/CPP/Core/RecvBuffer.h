#pragma once
#include <windows.h>
#include <vector>

class RecvBuffer
{
	enum {BUFFER_COUNT = 10};
public:
	RecvBuffer(INT32 bufferSize);
	~RecvBuffer();

	void			Clean();
	bool			OnRead(INT32 numOfBytes);
	bool			OnWrite(INT32 numOfBytes);

	BYTE*			ReadPos() { return &mBuffer[mReadPos]; }
	BYTE*			WritePos() { return &mBuffer[mWritePos]; }
	INT32			DataSize() { return mWritePos - mReadPos; }
	INT32			FreeSize() { return mCapacity - mWritePos; }

private:
	INT32			mCapacity = 0;
	INT32			mBufferSize = 0;
	INT32			mReadPos = 0;
	INT32			mWritePos = 0;
	std::vector<BYTE>	mBuffer;
};