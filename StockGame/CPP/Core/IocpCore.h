#pragma once
#include "IocpObject.h"

class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	HANDLE		GetHandle() { return mIocpHandle; }

	bool		Register(std::shared_ptr<IocpObject> iocpObject);
	bool		Dispatch(UINT32 timeoutMs = INFINITE);

private:
	HANDLE		mIocpHandle;
};