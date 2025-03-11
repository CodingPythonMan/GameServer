#include "Server.h"
#include <memory>

int main()
{
	auto server = std::make_shared<Server>();
	server->Start(L"127.0.0.1", 1231);
}