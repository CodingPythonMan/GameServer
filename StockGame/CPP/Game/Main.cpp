#include "Service.h"
#include "Memory.h"
#include "GameSession.h"

int main()
{
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 1537),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>,
		100);


}