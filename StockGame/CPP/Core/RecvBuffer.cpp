#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(int32 bufferSize) : mBufferSize(bufferSize)
{
	mCapacity = bufferSize * BUFFER_COUNT;
	mBuffer.resize(mCapacity);
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::Clean()
{
	int32 dataSize = GetDataSize();
	if (dataSize == 0)
	{
		// 마침 읽기*쓰기 커서가 동일한 위치라면, 둘다 리셋
		mReadPos = mWritePos = 0;
	}
	else
	{
		// 여유 공간이 버퍼 1개 크기 미만이면, 데이터를 앞으로 땡긴다.
		if (GetFreeSize() < mBufferSize)
		{
			::memcpy(&mBuffer[0], &mBuffer[mReadPos], dataSize);
			mReadPos = 0;
			mWritePos = dataSize;
		}
	}
}

bool RecvBuffer::OnRead(int32 numOfBytes)
{
	if (numOfBytes > GetDataSize())
		return false;

	mReadPos += numOfBytes;
	return true;
}

bool RecvBuffer::OnWrite(int32 numOfBytes)
{
	if (numOfBytes > GetFreeSize())
		return false;

	mWritePos += numOfBytes;
	return true;
}
