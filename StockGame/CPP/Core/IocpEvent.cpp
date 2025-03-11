#include "IocpEvent.h"

IocpEvent::IocpEvent(EventType type)
{
	Reset();
}

void IocpEvent::Reset()
{
	// 이벤트 객체 핸들
	OVERLAPPED::hEvent = 0;
	// Windows가 I/O 요청의 상태를 저장하는 내부 변수입니다.
	OVERLAPPED::Internal = 0;
	// I/O 작업 완료 후 전송된 바이트 수를 저장합니다.
	OVERLAPPED::InternalHigh = 0;
	// 파일 오프셋
	OVERLAPPED::Offset = 0;
	// I/O 작업 완료 후 전송된 바이트 수를 저장합니다.
	OVERLAPPED::OffsetHigh = 0;
}