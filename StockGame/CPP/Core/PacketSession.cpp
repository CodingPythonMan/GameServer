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
		// �ּ��� ����� �Ľ��� �� �־�� �Ѵ�.
		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
		// ����� ��ϵ� ��Ŷ ũ�⸦ �Ľ��� �� �־�� �Ѵ�.
		if (dataSize < header.size)
			break;

		// ��Ŷ ���� ����
		OnRecvPacket(&buffer[0], header.size);

		// ����͸� ����
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