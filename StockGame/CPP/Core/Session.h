#pragma once
#include <queue>
#include "NetAddress.h"
#include "IocpCore.h"
#include "IocpEvent.h"
#include "RecvBuffer.h"
#include "Macro.h"

class Service;

class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

	enum
	{
		BUFFER_SIZE = 0x10000, // 64kb
	};

public:
	Session();
	virtual ~Session();

public:
	// �ܺο��� ���
	void				Send(SendBufferRef sendBuffer);
	bool				Connect();
	void				Disconnect(const WCHAR* cause);

	std::shared_ptr<Service>	GetService() { return mService.lock(); }
	void						SetService(std::shared_ptr<Service> service) { mService = service; }

public:
	// ���� ����
	void				SetNetAddress(NetAddress address) { mNetAddress = address; }
	NetAddress			GetAddress() { return mNetAddress; }
	SOCKET				GetSocket() { return mClientSocket; }
	bool				IsConnected() { return mConnected; }
	SessionRef			GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }

private:
	// �������̽� ����
	virtual HANDLE		GetHandle() override;
	virtual void		Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	// ���� ����
	bool				RegisterConnect();
	bool				RegisterDisconnect();
	void				RegisterRecv();
	void				RegisterSend();

	void				ProcessConnect();
	void				ProcessDisconnect();
	void				ProcessRecv(int32 numOfBytes);
	void				ProcessSend(int32 numOfBytes);

	void				HandleError(int32 errorCode);
	
protected:
	// ������ �ڵ忡�� ������
	virtual void		OnConnected() {}
	virtual int32		OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void		OnSend(int32 len) {}
	virtual void		OnDisconnected() {}

private:
	std::weak_ptr<Service>	mService;
	SOCKET					mClientSocket = INVALID_SOCKET;
	NetAddress				mNetAddress = {};
	std::atomic<bool>		mConnected = false;

private:
	USE_LOCK;

	// ���� ����
	RecvBuffer					mRecvBuffer;
	
	// �۽� ����
	std::queue<SendBufferRef>	mSendQueue;
	std::atomic<bool>			mSendRegistered = false;

private:
	// IocpEvent ����
	ConnectEvent		mConnectEvent;
	DisconnectEvent		mDisconnectEvent;
	RecvEvent			mRecvEvent;
	SendEvent			mSendEvent;
};