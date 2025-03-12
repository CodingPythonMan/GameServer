#include "RingBuffer.h"
#include <cstring>
#include <algorithm>

RingBuffer::RingBuffer() : BufferSize{ RING_DEFAULT_SIZE }
{
    Buffer = new char[BufferSize + 1];
    Front = 0;
    Rear = 0;
}

RingBuffer::RingBuffer(int bufferSize) : BufferSize{ bufferSize }
{
    Buffer = new char[BufferSize + 1];
    Front = 0;
    Rear = 0;
}

RingBuffer::~RingBuffer()
{
    delete[] Buffer;
}

int RingBuffer::GetBufferSize()
{
    return BufferSize;
}

int RingBuffer::GetUseSize()
{
    return (Rear - Front + BufferSize + 1) % (BufferSize + 1);
}

int RingBuffer::GetFreeSize()
{
    return BufferSize - GetUseSize();
}

int RingBuffer::Enqueue(char* src, int size)
{
    if (size <= 0 || size > GetFreeSize())
        return 0;

    int firstChunk = std::min(size, BufferSize + 1 - Rear);
    int secondChunk = size - firstChunk;

    memcpy(&Buffer[Rear], src, firstChunk);
    if (secondChunk > 0)
        memcpy(&Buffer[0], src + firstChunk, secondChunk);

    Rear = (Rear + size) % (BufferSize + 1);
    return size;
}

int RingBuffer::Dequeue(char* dest, int size)
{
    if (size <= 0 || size > GetUseSize())
        return 0;

    int firstChunk = std::min(size, BufferSize + 1 - Front);
    int secondChunk = size - firstChunk;

    memcpy(dest, &Buffer[Front], firstChunk);
    if (secondChunk > 0)
        memcpy(dest + firstChunk, &Buffer[0], secondChunk);

    Front = (Front + size) % (BufferSize + 1);
    return size;
}

int RingBuffer::Peek(char* dest, int size)
{
    if (size <= 0 || size > GetUseSize())
        return 0;

    int firstChunk = std::min(size, BufferSize + 1 - Front);
    int secondChunk = size - firstChunk;

    memcpy(dest, &Buffer[Front], firstChunk);
    if (secondChunk > 0)
        memcpy(dest + firstChunk, &Buffer[0], secondChunk);

    return size;
}

void RingBuffer::ClearBuffer()
{
    Front = Rear;
}

int RingBuffer::DirectEnqueueSize()
{
    if (Rear >= Front)
        return BufferSize + 1 - Rear;
    else
        return Front - Rear - 1;
}

int RingBuffer::DirectDequeueSize()
{
    if (Front <= Rear)
        return Rear - Front;
    else
        return BufferSize + 1 - Front;
}

int RingBuffer::MoveRear(int size)
{
    if (size <= 0 || size > GetFreeSize())
        return 0;

    Rear = (Rear + size) % (BufferSize + 1);
    return size;
}

int RingBuffer::MoveFront(int size)
{
    if (size <= 0 || size > GetUseSize())
        return 0;

    Front = (Front + size) % (BufferSize + 1);
    return size;
}

char* RingBuffer::GetFrontBufferPtr()
{
    return &Buffer[Front];
}

char* RingBuffer::GetRearBufferPtr()
{
    return &Buffer[Rear];
}

char* RingBuffer::GetStartBufferPtr()
{
    return Buffer;
}