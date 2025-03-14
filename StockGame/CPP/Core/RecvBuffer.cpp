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
		// ��ħ �б�*���� Ŀ���� ������ ��ġ���, �Ѵ� ����
		mReadPos = mWritePos = 0;
	}
	else
	{
		// ���� ������ ���� 1�� ũ�� �̸��̸�, �����͸� ������ �����.
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
