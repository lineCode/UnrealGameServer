#include "pch.h"
#include "ConverString.h"

ConverString::ConverString(string str) 
{
	_cstr = const_cast<CHAR*>(str.c_str());

	wchar_t* pStr;

	int strSize = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, NULL);

	pStr = new WCHAR[strSize];

	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size() + 1, pStr, strSize);

	_tstr = pStr;

	_size = strSize - 1;
}

ConverString::ConverString(const CHAR* str) 
{
	_cstr = const_cast<CHAR*>(str);
	wchar_t* pStr;

	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);

	pStr = new WCHAR[strSize];

	MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, pStr, strSize);

	wstring src = pStr;

	_tstr = pStr;

	_size = src.size();
}

ConverString::ConverString(CHAR* str) : _cstr(str)
{
	wchar_t* pStr;

	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);

	pStr = new WCHAR[strSize];

	MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, pStr, strSize);

	wstring src = pStr;

	_tstr = pStr;

	_size = src.size();
}

ConverString::ConverString(const WCHAR* str)
{
	_tstr = const_cast<WCHAR*>(str);
	//반환할 char* 변수 선언
	char* pStr;

	//입력받은 wchar_t 변수의 길이를 구함
	int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	//char* 메모리 할당
	pStr = new char[strSize];

	//형 변환 
	WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);
	_cstr = pStr;
	_size = strSize - 1;
}

ConverString::ConverString(WCHAR* str) : _tstr(str)
{
	//반환할 char* 변수 선언
	char* pStr;

	//입력받은 wchar_t 변수의 길이를 구함
	int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	//char* 메모리 할당
	pStr = new char[strSize];

	//형 변환 
	WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);
	_cstr = pStr;
	_size = strSize - 1;
}
