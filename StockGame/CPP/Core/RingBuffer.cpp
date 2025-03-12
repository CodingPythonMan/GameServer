#include "RingBuffer.h"
#include <cstring>
#include <algorithm>

RingBuffer::RingBuffer() : mBufferSize{ RING_DEFAULT_SIZE }
{
	mBuffer = new char[mBufferSize + 1];
	mFront = 0;
	mRear = 0;
}

RingBuffer::RingBuffer(int bufferSize) : mBufferSize{ bufferSize }
{
	mBuffer = new char[mBufferSize + 1];
	mFront = 0;
	mRear = 0;
}

RingBuffer::~RingBuffer()
{
	delete[] mBuffer;
}

int RingBuffer::GetBufferSize()
{
	return mBufferSize;
}

int RingBuffer::GetUseSize()
{
	return (mRear - mFront + mBufferSize + 1) % (mBufferSize + 1);
}

int RingBuffer::GetFreeSize()
{
	return mBufferSize - GetUseSize();
}

int RingBuffer::Enqueue(char* src, int size)
{
	if (size <= 0 || size > GetFreeSize())
		return 0;

	int firstChunk = std::min(size, mBufferSize + 1 - mRear);
	int secondChunk = size - firstChunk;

	::memcpy(&mBuffer[mRear], src, firstChunk);
	if (secondChunk > 0)
	{
		::memcpy(&mBuffer[0], src + firstChunk, secondChunk);
	}

	mRear = (mRear + size) % (mBufferSize + 1);
	return size;
}

int RingBuffer::Dequeue(char* dest, int size)
{
	if (size <= 0 || size > GetUseSize())
	{
		return 0;
	}

	int firstChunk = std::min(size, mBufferSize + 1 - mFront);
	int secondChunk = size - firstChunk;

	::memcpy(dest, &mBuffer[mFront], firstChunk);
	if (secondChunk > 0)
	{
		::memcpy(dest + firstChunk, &mBuffer[0], secondChunk);
	}

	mFront = (mFront + size) % (mBufferSize + 1);
	return size;
}

int RingBuffer::Peek(char* dest, int size)
{
	if (size <= 0 || size > GetUseSize())
		return 0;

	int firstChunk = std::min(size, mBufferSize + 1 - mFront);
	int secondChunk = size - firstChunk;

	::memcpy(dest, &mBuffer[mFront], firstChunk);
	if (secondChunk > 0)
	{
		::memcpy(dest + firstChunk, &mBuffer[0], secondChunk);
	}

	return size;
}

void RingBuffer::ClearBuffer()
{
	mFront = mRear;
}

int RingBuffer::DirectEnqueueSize()
{
	if (mRear >= mFront)
	{
		return mBufferSize + 1 - mRear;
	}
	
	return mFront - mRear - 1;
}

int RingBuffer::DirectDequeueSize()
{
	if (mFront <= mRear)
	{
		return mRear - mFront;
	}
	
	return mBufferSize + 1 - mFront;
}

int RingBuffer::MoveRear(int size)
{
	if (size <= 0 || size > GetFreeSize())
	{
		return 0;
	}

	mRear = (mRear + size) % (mBufferSize + 1);
	return size;
}

int RingBuffer::MoveFront(int size)
{
	if (size <= 0 || size > GetUseSize())
	{
		return 0;
	}

	mFront = (mFront + size) % (mBufferSize + 1);
	return size;
}

char* RingBuffer::GetFrontBufferPtr()
{
	return &mBuffer[mFront];
}

char* RingBuffer::GetRearBufferPtr()
{
	return &mBuffer[mRear];
}

char* RingBuffer::GetStartBufferPtr()
{
	return mBuffer;
}