#include "DBService.h"
#include "DBConnectionInfo.h"
#include "ConsoleLog.h"
#include "CoreGlobal.h"
#include "Poco/Data/ODBC/Connector.h"
#include "Poco/Data/ODBC/Utility.h"
#include "Poco/Data/Session.h"
#include "Poco/Data/Statement.h"
#include "Poco/Data/ODBC/ODBCException.h"

using namespace Poco::Data::Keywords;

void DBService::Start()
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
	
	Poco::Data::ODBC::Utility::DriverMap drivers;
	Poco::Data::ODBC::Utility::drivers(drivers);

	Poco::Data::Session* pNewSession = new Poco::Data::Session(Poco::Data::ODBC::Connector::KEY, connectionStr, 5);
	pNewSession->impl()->setProperty("maxFieldSize", Poco::Any(4096));

	if (pNewSession->isConnected())
	{
		GConsoleLogger->WriteStdOut(Color::BLACK, L"DB Connected ::[%s]", info.DBName.c_str());
	}

	try
	{
		//*pNewSession << "{CALL spBasic(?)}",
		//	bind(1), 
		//	now;

		//Poco::Data::Statement state(*pNewSession);
		//state << std::format("EXEC spBasic {}", 1);
		//state.execute();

		*pNewSession << "EXEC spBasic ?",
			bind(1),
			now;
	}
	catch (Poco::Data::ODBC::StatementException& ex)
	{
		printf("\n%s", ex.message().c_str());
	}
	catch (std::exception& e)
	{
		printf("\nDB Error : %s", e.what());
	}
}