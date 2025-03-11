#include "Server.h"
#include "Macro.h"
#include "IocpEventPool.h"

void Server::Start(const WCHAR ip[], int port, bool nagle)
{
	// 4번째 인자가 0이면 Windows 에서 알아서 잡아줌.
	mIocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(mIocp != INVALID_HANDLE_VALUE);

	// init
	WSADATA wsa;
	ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), &wsa) == 0);

	mListenSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	ASSERT_CRASH(mListenSocket != INVALID_SOCKET);

	// 이 곳에서 소켓과 IOCP 를 연결해주는 작업
	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(mListenSocket), mIocp, /*Key*/0, 0);

	_SetBaseSocketOption(nagle);

	// bind
	_Bind(ip, port);

	// listen
	_Listen();

	// accept 하는 스레드를 따로 뗄 것인가.
	for (int i = 0; i < 5; i++)
	{
		auto acceptEvent = IocpEventPool::GetInstance().Alloc<AcceptEvent>();
		acceptEvent->mOwner = this;
		acceptEvent->mEventType = EventType::ACCEPT;
		mAcceptEventList.push_back(acceptEvent);
		_RegisterAccept(acceptEvent);
	}
}

void Server::Clear()
{
	::WSACleanup();
}

void Server::_SetBaseSocketOption(bool nagle)
{
	// 링거 옵션 셋팅 - 4 way handshake
	linger Linger;
	Linger.l_onoff = 1;
	Linger.l_linger = 0;
	::setsockopt(mListenSocket, SOL_SOCKET, SO_LINGER, (char*)&Linger, sizeof(Linger));

	// 기본 Nagle 설정
	if (nagle == false)
	{
		int nodelay = 1;
		::setsockopt(mListenSocket, SOL_SOCKET, TCP_NODELAY, (const char*)&nodelay, sizeof(nodelay));
	}
}

void Server::_Bind(const WCHAR ip[], int port)
{
	SOCKADDR_IN listenAddr;
	::memset(&listenAddr, 0, sizeof(listenAddr));
	listenAddr.sin_family = AF_INET;
	InetPton(AF_INET, ip, &listenAddr.sin_addr);
	listenAddr.sin_port = htons(port);
	int retval = ::bind(mListenSocket, (SOCKADDR*)&listenAddr, sizeof(listenAddr));

	ASSERT_CRASH(retval != SOCKET_ERROR);
}

void Server::_Listen()
{
	int retval = ::listen(mListenSocket, SOMAXCONN);
	ASSERT_CRASH(retval != SOCKET_ERROR);
}

void Server::_RegisterAccept(std::shared_ptr<AcceptEvent> acceptEvent)
{
	// Session 을 생성해야 한다.
	//\
	// Session* session = SessionManager::GetInstance().CreateSession();
	
	//acceptEvent->mSession = session;
}