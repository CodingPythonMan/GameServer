#pragma once
#include <queue>
#include <mutex>
#include <memory>
#include <functional>
#include "IocpEvent.h"
#include "RefSingleton.h"

class IocpEventPool : public RefSingleton<IocpEventPool>
{
public:
    IocpEventPool() = default;

    // 소멸자에서 풀을 정리
    ~IocpEventPool()
    {
        while (false == mPool.empty())
        {
            delete mPool.front();
            mPool.pop();
        }
    }

    // 객체를 스마트 포인터로 반환하여 자동 관리되도록 설정
    template <typename T>
    std::shared_ptr<T> Alloc()
    {
        std::lock_guard<std::mutex> lock(mMutex);

        if (!mPool.empty())
        {
            IocpEvent* event = mPool.front();
            mPool.pop();
            return { static_cast<T*>(event), [](T* e) { IocpEventPool::GetInstance().Release(e); } };
        }

        return { new T(), [](T* e) { IocpEventPool::GetInstance().Release(e); } };
    }

    // 객체 반납
    void Release(IocpEvent* event)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        event->Reset(); // 초기화 후 다시 풀에 삽입
        mPool.push(event);
    }

private:
    std::queue<IocpEvent*> mPool;
    std::mutex mMutex;
};