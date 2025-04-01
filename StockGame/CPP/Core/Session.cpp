#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"
#include "CoreGlobal.h"
#include "ConsoleLog.h"
#include "SendBuffer.h"
#include <iostream>

Session::Session() : mRecvBuffer(BUFFER_SIZE)
{
	mClientSocket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	SocketUtils::Close(mClientSocket);
}

void Session::Send(SendBufferRef sendBuffer)
{
	if (IsConnected() == false)
	{
		return;
	}

	bool registerSend = false;

	// 현재 RegisterSend 가 걸리지 않은 상태라면, 걸어준다.
	{
		WRITE_LOCK;

		mSendQueue.push(sendBuffer);

		if (mSendRegistered.exchange(true) == false)
		{
			registerSend = true;
		}
	}

	if (true == registerSend)
	{
		RegisterSend();
	}
}

bool Session::Connect()
{
	return RegisterConnect();
}

void Session::Disconnect(const WCHAR* cause)
{
	if (mConnected.exchange(false) == false)
	{
		return;
	}

	GConsoleLogger->WriteStdOut(Color::WHITE, L"Disconnect : %s\n", cause);

	RegisterDisconnect();
}

HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(mClientSocket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->mEventType)
	{
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Disconnect:
		ProcessDisconnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(numOfBytes);
		break;
	default:
		break;
	}
}

bool Session::RegisterConnect()
{
	if (true == IsConnected())
	{
		return false;
	}

	if (GetService()->GetServiceType() != ServiceType::Client)
	{
		return false;
	}

	if (SocketUtils::SetReuseAddress(mClientSocket, true) == false)
	{
		return false;
	}

	if (SocketUtils::BindAnyAddress(mClientSocket, 0) == false)
	{
		return false;
	}

	mConnectEvent.Initialize();
	mConnectEvent.mOwner = shared_from_this(); // ADD_REF

	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
	if (false == SocketUtils::ConnectEx(mClientSocket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr),
		nullptr, 0, &numOfBytes, &mConnectEvent))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			mConnectEvent.mOwner = nullptr;
			return false;
		}
	}

	return true;
}

bool Session::RegisterDisconnect()
{
	mDisconnectEvent.Initialize();
	mDisconnectEvent.mOwner = shared_from_this();

	if (false == SocketUtils::DisconnectEx(mClientSocket, &mDisconnectEvent, TF_REUSE_SOCKET, 0))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			mDisconnectEvent.mOwner = nullptr;
			return false;
		}
	}

	return true;
}

void Session::RegisterRecv()
{
	if (IsConnected() == false)
	{
		return;
	}

	mRecvEvent.Initialize();
	mRecvEvent.mOwner = shared_from_this();

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(mRecvBuffer.GetWritePos());
	wsaBuf.len = mRecvBuffer.GetFreeSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (SOCKET_ERROR == ::WSARecv(mClientSocket, &wsaBuf, 1, OUT & numOfBytes, OUT & flags, &mRecvEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			mRecvEvent.mOwner = nullptr; // RELEASE_REF
		}
	}
}

void Session::RegisterSend()
{
	if (IsConnected() == false)
	{
		return;
	}

	mSendEvent.Initialize();
	mSendEvent.mOwner = shared_from_this();

	// 보낼 데이터를 sendEvent에 등록
	{
		WRITE_LOCK;

		int32 writeSize = 0;
		while (mSendQueue.empty() == false)
		{
			SendBufferRef sendBuffer = mSendQueue.front();

			writeSize += sendBuffer->WriteSize();
			// TODO: 예외 체크

			mSendQueue.pop();
			mSendEvent.mSendBufferList.push_back(sendBuffer);
		}
	}

	// Scatter-Gather(흩어져있는 데이터들을 모아서 한방에 보낸다.)
	std::vector<WSABUF> wsaBufs;
	wsaBufs.reserve(mSendEvent.mSendBufferList.size());
	for (SendBufferRef sendBuffer : mSendEvent.mSendBufferList)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
		wsaBuf.len = static_cast<LONG>(sendBuffer->WriteSize());
		wsaBufs.push_back(wsaBuf);
	}

	DWORD numOfBytes = 0;
	if (SOCKET_ERROR == ::WSASend(mClientSocket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT & numOfBytes, 0, &mSendEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			mSendEvent.mOwner = nullptr; // release ref
			mSendEvent.mSendBufferList.clear(); // release ref
			mSendRegistered.store(false);
		}
	}
}

void Session::ProcessConnect()
{
	mConnectEvent.mOwner = nullptr; // Release ref

	mConnected.store(true);

	// 세션 등록
	GetService()->AddSession(GetSessionRef());

	// 컨텐츠 코드에서 재정의
	OnConnected();

	// 수신 등록
	RegisterRecv();
}

void Session::ProcessDisconnect()
{
	mDisconnectEvent.mOwner = nullptr;

	OnDisconnected(); // 컨텐츠 코드에서 재정의
	GetService()->ReleaseSession(GetSessionRef());
}

void Session::ProcessRecv(int32 numOfBytes)
{
	mRecvEvent.mOwner = nullptr; // RELEASE_REF

	if (numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	if (mRecvBuffer.OnWrite(numOfBytes) == false)
	{
		Disconnect(L"OnWrite Overflow");
		return;
	}

	int32 dataSize = mRecvBuffer.GetDataSize();
	int32 processLen = OnRecv(mRecvBuffer.GetReadPos(), dataSize);
	if (processLen < 0 || dataSize < processLen || mRecvBuffer.OnRead(processLen) == false)
	{
		Disconnect(L"OnRead Overflow");
		return;
	}

	// 커서 정리
	mRecvBuffer.Clean();
	
	// 수신 등록
	RegisterRecv();
}

void Session::ProcessSend(int32 numOfBytes)
{
	mSendEvent.mOwner = nullptr;
	mSendEvent.mSendBufferList.clear();

	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	// 컨텐츠 코드에서 재정의
	OnSend(numOfBytes);

	WRITE_LOCK;
	if (mSendQueue.empty())
	{
		mSendRegistered.store(false);
	}
	else
	{
		RegisterSend();
	}
}

void Session::HandleError(int32 errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"Handle Error");
		break;
	default:
		GConsoleLogger->WriteStdOut(Color::WHITE, L"Handle Error : %d\n", errorCode);
		break;
	}
}