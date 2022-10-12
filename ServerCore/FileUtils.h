#pragma once

/*---------------------------------------------------------------------------------------------
이름     : FileUtils
용도     : File 입출력을 편하게 돕는 유틸리티 객체
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
class FileUtils
{
public:
	static Gvector<BYTE> ReadFile(const WCHAR* path);
	static GWString	Convert(string str);
};