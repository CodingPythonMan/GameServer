#pragma once
#include "RefSingleton.h"
#include "Macro.h"
#include <vector>

namespace Poco { namespace Data { class Session; } }

class DBManager : public RefSingleton<DBManager>
{
public:
    // �ʱ�ȭ: connectionStr�� ����� ���� ������ ����
    void                    Init(int poolSize = 10);

protected:
    void                    _Connect(const std::string& connectionStr, int poolSize);

    // ȣ���� ������ �ٸ� ������ ��ȯ
    Poco::Data::Session&    _GetSession();

private:
    std::vector<std::unique_ptr<Poco::Data::Session>>    mSessionList;
    std::atomic<int>                    mCounter{ 0 };
};