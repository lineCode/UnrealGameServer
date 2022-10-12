#pragma once

enum class Color
{
	BLACK,
	WHITE,
	RED,
	GREEN,
	BLUE,
	YELLOW,
};

/*---------------------------------------------------------------------------------------------
이름     : ConsoleLog
용도     : 콘솔의 로그에 색상을 설정 후 출력해주는 객체
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
class ConsoleLog
{
	enum { BUFFER_SIZE = 4096 };

public:
	ConsoleLog();
	~ConsoleLog();

public:
	void		WriteStdOut(Color color, const WCHAR* str, ...);
	void		WriteStdErr(Color color, const WCHAR* str, ...);

protected:
	void		SetColor(bool stdOut, Color color);

private:
	HANDLE		_stdOut;
	HANDLE		_stdErr;
};