#include "PacketSession.h"
#include "CoreTLS.h"
#include "Monitoring.h"

PacketSession::PacketSession()
{
}

PacketSession::~PacketSession()
{
}

int32 PacketSession::OnRecv(BYTE* buffer, int32 len)
{
	int32 processLen = 0;

	while (true)
	{
		int32 dataSize = len - processLen;
		// 최소한 헤더는 파싱할 수 있어야 한다.
		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
		// 헤더에 기록된 패킷 크기를 파싱할 수 있어야 한다.
		if (dataSize < header.size)
			break;

		// 패킷 조립 성공
		OnRecvPacket(&buffer[0], header.size);

		// 모니터링 증가
		_IncreaseRecvCount();

		processLen += header.size;
	}

	return processLen;
}

void PacketSession::_IncreaseRecvCount()
{
	if (LMonitorData != nullptr)
	{
		if (LMonitorData->mRecvCount > 200000000)
		{
			Monitoring::GetInstance().ResetRecvSnapshot(LMonitorData, LMonitorData->mRecvCount);
		}

		LMonitorData->mRecvCount++;
	}
}