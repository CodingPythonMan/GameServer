#include "Server.h"

int main()
{
	auto server = std::make_shared<Server>();
	server->Start(L"127.0.0.1", 1231);

	while (1)
	{
		// �켱 ���� �����忡���� Accept �� �޴� ������ �����غ���.
		server->Update();
		Sleep(300);
	}
}