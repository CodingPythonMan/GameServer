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

    // 소멸자에서 풀을 정리
    ~SessionPool()
    {
        while (false == mPool.empty())
        {
            delete mPool.front();
            mPool.pop();
        }
    }

    // 객체를 스마트 포인터로 반환하여 자동 관리되도록 설정
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

    // 객체 반납
    void Release(Session* session)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        session->Reset(); // 초기화 후 다시 풀에 삽입
        mPool.push(session);
    }

private:
    std::queue<Session*> mPool;
    std::mutex mMutex;
};