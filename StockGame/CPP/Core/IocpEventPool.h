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

    // �Ҹ��ڿ��� Ǯ�� ����
    ~IocpEventPool()
    {
        while (false == mPool.empty())
        {
            delete mPool.front();
            mPool.pop();
        }
    }

    // ��ü�� ����Ʈ �����ͷ� ��ȯ�Ͽ� �ڵ� �����ǵ��� ����
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

    // ��ü �ݳ�
    void Release(IocpEvent* event)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        event->Reset(); // �ʱ�ȭ �� �ٽ� Ǯ�� ����
        mPool.push(event);
    }

private:
    std::queue<IocpEvent*> mPool;
    std::mutex mMutex;
};