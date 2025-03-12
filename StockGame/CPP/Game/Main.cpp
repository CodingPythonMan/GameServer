#include "Server.h"

int main()
{
	auto server = std::make_shared<Server>();
	server->Start(L"127.0.0.1", 1231);

	while (1)
	{
		// 우선 메인 스레드에서만 Accept 를 받는 업무를 진행해보자.
		server->Update();
		Sleep(300);
	}
}