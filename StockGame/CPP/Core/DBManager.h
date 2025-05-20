#pragma once
#include "RefSingleton.h"
#include "Macro.h"
#include <vector>

namespace Poco { namespace Data { class Session; } }

class DBManager : public RefSingleton<DBManager>
{
public:
    // 초기화: connectionStr과 사용할 세션 개수를 지정
    void                    Init(int poolSize = 10);

protected:
    void                    _Connect(const std::string& connectionStr, int poolSize);

    // 호출할 때마다 다른 세션을 반환
    Poco::Data::Session&    _GetSession();

private:
    std::vector<std::unique_ptr<Poco::Data::Session>>    mSessionList;
    std::atomic<int>                    mCounter{ 0 };
};