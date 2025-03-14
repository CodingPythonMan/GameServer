#pragma once
#include <memory>
#include <windows.h>

class IocpObject : public std::enable_shared_from_this<IocpObject>
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* iocpEvent, INT32 numOfBytes = 0) abstract;
};