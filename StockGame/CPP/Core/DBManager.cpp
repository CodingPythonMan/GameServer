#include "DBManager.h"
#include "DBConnectionInfo.h"
#include "ConsoleLog.h"
#include "CoreGlobal.h"
#include "Poco/Data/ODBC/Connector.h"
#include "Poco/Data/ODBC/Utility.h"
#include "Poco/Data/Session.h"
#include "Poco/Data/Statement.h"
#include "Poco/Data/ODBC/ODBCException.h"

using namespace Poco::Data::Keywords;

void DBManager::Init(int poolSize)
{
	DBConnectionInfo info;
	info.Host = "127.0.0.1";
	info.Port = 1433;
	info.UserID = "Robo";
	info.UserPW = "1234";  // 숫자가 아니라 문자열로 처리
	info.DBName = "Test";

	std::string driver = "SQL Server";

	std::string connectionStr =
		"DRIVER=" + driver +
		";UID=" + info.UserID +
		";PWD=" + info.UserPW +
		";DATABASE=" + info.DBName +
		";SERVER=" + info.Host + "," + std::to_string(info.Port) + ";";

	printf("Connection String: %s\n", connectionStr.c_str());

	Poco::Data::ODBC::Connector::registerConnector();

	_Connect(connectionStr, poolSize);
}

void DBManager::_Connect(const std::string& connectionStr, int poolSize)
{
	Poco::Data::ODBC::Utility::DriverMap drivers;
	Poco::Data::ODBC::Utility::drivers(drivers);

	for (int i = 0; i < poolSize; i++)
	{
		auto sess = std::make_unique<Poco::Data::Session>(
			Poco::Data::ODBC::Connector::KEY, connectionStr);
		sess->impl()->setProperty("maxFieldSize", Poco::Any(8192));
		mSessionList.emplace_back(std::move(sess));
	}
}

Poco::Data::Session& DBManager::_GetSession()
{
	int index = mCounter.fetch_add(1, std::memory_order_relaxed) % mSessionList.size();
	return *mSessionList[index];
}
