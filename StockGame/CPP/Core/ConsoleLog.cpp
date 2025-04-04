#include "ConsoleLog.h"
#include <iostream>
#include <chrono>
#include <ctime>

ConsoleLog::ConsoleLog()
{
	_stdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
	_stdErr = ::GetStdHandle(STD_ERROR_HANDLE);
}

ConsoleLog::~ConsoleLog()
{
}

void ConsoleLog::WriteStdOut(Color color, const WCHAR* format, ...)
{
	if (format == nullptr)
		return;

	SetColor(true, color);

	// 타임스탬프 출력
	std::wstring timestamp = _GetCurrentTimestamp();
	std::wcout << timestamp;

	va_list ap;
	va_start(ap, format);
	::vwprintf(format, ap);
	va_end(ap);

	fflush(stdout);

	SetColor(true, Color::WHITE);
}

void ConsoleLog::WriteStdErr(Color color, const WCHAR* format, ...)
{
	WCHAR buffer[BUFFER_SIZE];

	if (format == nullptr)
		return;

	SetColor(false, color);

	// 타임스탬프 출력
	std::wstring timestamp = _GetCurrentTimestamp();
	std::wcout << timestamp;

	va_list ap;
	va_start(ap, format);
	::vswprintf_s(buffer, BUFFER_SIZE, format, ap);
	va_end(ap);

	::fwprintf_s(stderr, buffer);
	fflush(stderr);

	SetColor(false, Color::WHITE);
}

void ConsoleLog::SetColor(bool stdOut, Color color)
{
	static WORD SColors[]
	{
		0,
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		FOREGROUND_RED | FOREGROUND_INTENSITY,
		FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
	};

	::SetConsoleTextAttribute(stdOut ? _stdOut : _stdErr, SColors[static_cast<INT32>(color)]);
}

std::wstring ConsoleLog::_GetCurrentTimestamp()
{
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);

	std::tm time_info;
#ifdef _WIN32
	localtime_s(&time_info, &now_c);
#else
	localtime_r(&now_c, &time_info);
#endif

	wchar_t buffer[32];
	wcsftime(buffer, sizeof(buffer) / sizeof(wchar_t), L"[%Y-%m-%d %H:%M:%S] ", &time_info);
	return std::wstring(buffer);
}