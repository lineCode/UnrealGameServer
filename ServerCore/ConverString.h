#pragma once

/*---------------------------------------------------------------------------------------------
이름     : ConverString
용도     : CHAR과 WCHAR형의 형변환이 자유로운 String
수정자   : 이민규
수정날짜 : 2022.10.16
----------------------------------------------------------------------------------------------*/
class ConverString
{
public:
	ConverString(string str);
	ConverString(const CHAR* str);
	ConverString(CHAR* str);
	ConverString(const WCHAR* str);
	ConverString(WCHAR* str);

	CHAR* GetCHAR() { return _cstr; }
	WCHAR* GetWCHAR() { return _tstr; }
	int32 GetSize() { return _size; }
private:
	CHAR* _cstr = nullptr;
	WCHAR* _tstr = nullptr;
	int32 _size = 0;
};

