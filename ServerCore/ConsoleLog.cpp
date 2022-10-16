#include "pch.h"
#include "ConsoleLog.h"

ConsoleLog::ConsoleLog()
{
	_stdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
	_stdErr = ::GetStdHandle(STD_ERROR_HANDLE);
}

ConsoleLog::~ConsoleLog()
{
}

/*---------------------------------------------------------------------------------------------
이름     : ConsoleLog::WriteStdOut
용도     : 콘솔에 색상설정 후 로그 출력해주는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
void ConsoleLog::WriteStdOut(Color color, const WCHAR* format, ...)
{
	if (format == nullptr)
		return;

	SetColor(true, color);

	va_list ap;
	va_start(ap, format);
	::vwprintf(format, ap);
	va_end(ap);

	SetColor(true, Color::WHITE);
}

/*---------------------------------------------------------------------------------------------
이름     : ConsoleLog::WriteStdErr
용도     : 에러 메세지를 출력해주는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
void ConsoleLog::WriteStdErr(Color color, const WCHAR* format, ...)
{
	WCHAR buffer[BUFFER_SIZE];

	if (format == nullptr)
		return;

	SetColor(false, color);

	va_list ap;
	va_start(ap, format);
	::vswprintf_s(buffer, BUFFER_SIZE, format, ap);
	va_end(ap);

	::fwprintf_s(stderr, buffer);
	fflush(stderr);

	SetColor(false, Color::WHITE);
}

/*---------------------------------------------------------------------------------------------
이름     : ConsoleLog::SetColor
용도     : 콘솔에 로그 색상 설정하는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
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

	::SetConsoleTextAttribute(stdOut ? _stdOut : _stdErr, SColors[static_cast<int32>(color)]);
}