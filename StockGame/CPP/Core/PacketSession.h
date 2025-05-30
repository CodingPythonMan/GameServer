#pragma once
#include "Session.h"

struct PacketHeader
{
	uint16 size;
	uint16 id; // 프로토콜 ID (ex. 1=로그인, 2=이동유형) 
};

class PacketSession : public Session
{
public:
	PacketSession();
	virtual ~PacketSession();

	PacketSessionRef GetPacketSessionRef() { return static_pointer_cast<PacketSession>(shared_from_this()); }

protected:
	virtual int32	OnRecv(BYTE* buffer, int32 len) sealed;
	virtual void	OnRecvPacket(BYTE* buffer, int32 len) abstract;

	void			_IncreaseRecvCount();
};