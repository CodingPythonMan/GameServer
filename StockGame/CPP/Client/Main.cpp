#include "Client.h"

int main()
{
	auto client = std::make_shared<Client>();
	Sleep(10000);
	client->Start(L"127.0.0.1", 1231);

	while (1)
	{
		Sleep(1000);
	}
}