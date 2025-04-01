#include "ServerSession.h"
#include "Memory.h"
#include "Service.h"
#include "IocpCore.h"

int main()
{
	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 1537),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>,
		1);

	service->Start();


}