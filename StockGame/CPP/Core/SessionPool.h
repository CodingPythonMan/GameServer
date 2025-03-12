#pragma once
#include <queue>
#include <mutex>
#include <memory>
#include <functional>
#include "Session.h"
#include "RefSingleton.h"

class SessionPool : public RefSingleton<SessionPool>
{
public:
    SessionPool() = default;

    // �Ҹ��ڿ��� Ǯ�� ����
    ~SessionPool()
    {
        while (false == mPool.empty())
        {
            delete mPool.front();
            mPool.pop();
        }
    }

    // ��ü�� ����Ʈ �����ͷ� ��ȯ�Ͽ� �ڵ� �����ǵ��� ����
    std::shared_ptr<Session> Alloc()
    {
        std::lock_guard<std::mutex> lock(mMutex);

        if (!mPool.empty())
        {
            Session* session = mPool.front();
            mPool.pop();
            return { static_cast<Session*>(session), [](Session* s) { SessionPool::GetInstance().Release(s); } };
        }

        return { new Session(), [](Session* s) { SessionPool::GetInstance().Release(s); } };
    }

    // ��ü �ݳ�
    void Release(Session* session)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        session->Reset(); // �ʱ�ȭ �� �ٽ� Ǯ�� ����
        mPool.push(session);
    }

private:
    std::queue<Session*> mPool;
    std::mutex mMutex;
};