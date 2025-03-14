#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(INT32 bufferSize) : mBufferSize(bufferSize)
{
	mCapacity = bufferSize * BUFFER_COUNT;
	mBuffer.resize(bufferSize);
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::Clean()
{
	INT32 dataSize = DataSize();
	if (dataSize == 0)
	{
		// 마침 읽기*쓰기 커서가 동일한 위치라면, 둘다 리셋
		mReadPos = mWritePos = 0;
	}
	else
	{
		// 여유 공간이 버퍼 1개 크기 미만이면, 데이터를 앞으로 땡긴다.
		if (FreeSize() < mBufferSize)
		{
			::memcpy(&mBuffer[0], &mBuffer[mReadPos], dataSize);
			mReadPos = 0;
			mWritePos = dataSize;
		}
	}
}

bool RecvBuffer::OnRead(INT32 numOfBytes)
{
	if (numOfBytes > DataSize())
		return false;

	mReadPos += numOfBytes;
	return true;
}

bool RecvBuffer::OnWrite(INT32 numOfBytes)
{
	if (numOfBytes > FreeSize())
		return false;

	mWritePos += numOfBytes;
	return true;
}
