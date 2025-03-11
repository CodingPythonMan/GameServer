#include "IocpEvent.h"

IocpEvent::IocpEvent(EventType type)
{
	Reset();
}

void IocpEvent::Reset()
{
	// �̺�Ʈ ��ü �ڵ�
	OVERLAPPED::hEvent = 0;
	// Windows�� I/O ��û�� ���¸� �����ϴ� ���� �����Դϴ�.
	OVERLAPPED::Internal = 0;
	// I/O �۾� �Ϸ� �� ���۵� ����Ʈ ���� �����մϴ�.
	OVERLAPPED::InternalHigh = 0;
	// ���� ������
	OVERLAPPED::Offset = 0;
	// I/O �۾� �Ϸ� �� ���۵� ����Ʈ ���� �����մϴ�.
	OVERLAPPED::OffsetHigh = 0;
}