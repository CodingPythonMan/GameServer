#pragma once
#include <string>

struct DBConnectionInfo
{
	std::string Host = "";
	int Port = 0;
	std::string UserID = "";
	std::string UserPW = "";
	std::string DBName = "";
	int ThreadCount = 0;
	int ServerID = 0;

	bool IsValid()
	{
		if (true == Host.empty()
			|| true == UserID.empty()
			|| true == DBName.empty()
			|| 0 >= ThreadCount
			|| 0 >= Port
			|| 0x10000 <= Port)
			return false;
		return true;
	}
};