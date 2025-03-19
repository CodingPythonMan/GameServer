#include "DBService.h"
#include "DBConnectionInfo.h"
#include "Poco/Data/ODBC/Connector.h"
#include "Poco/Data/ODBC/Utility.h"
#include "Poco/Data/Session.h"
#include "ConsoleLog.h"
#include "CoreGlobal.h"

void DBService::Start()
{
	DBConnectionInfo info;
	info.Host = "127.0.0.1";
	info.Port = 1433;
	info.UserID = "Robo";
	info.UserPW = 1234;
	info.DBName = "Test";

	std::string driver = "SQL Server";

	char tempConnection[512] = { 0, };
	sprintf_s(tempConnection, std::size(tempConnection), "DRIVER=%s;UID=%s;PWD=%s;DATABASE=%s;SERVER=%s,%s;",
		driver.c_str(), info.UserID, info.UserPW, info.DBName, info.Host, info.Port);

	Poco::Data::ODBC::Connector::registerConnector();
	
	Poco::Data::ODBC::Utility::DriverMap drivers;
	Poco::Data::ODBC::Utility::drivers(drivers);

	Poco::Data::Session* pNewSession = new Poco::Data::Session(Poco::Data::ODBC::Connector::KEY, tempConnection, 5);
	pNewSession->impl()->setProperty("maxFieldSize", Poco::Any(4096));

	if (pNewSession->isConnected())
	{
		GConsoleLogger->WriteStdOut(Color::BLACK, L"DB Connected ::[%s]", info.DBName.c_str());
	}
	

}