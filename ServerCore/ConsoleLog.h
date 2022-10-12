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
�̸�     : ConsoleLog
�뵵     : �ܼ��� �α׿� ������ ���� �� ������ִ� ��ü
������   : �̹α�
������¥ : 2022.10.08
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