#pragma once

/*---------------------------------------------------------------------------------------------
�̸�     : FileUtils
�뵵     : File ����� �� ��ȯ�� ���ϰ� ���� ��ƿ��Ƽ ��ü
������   : �̹α�
������¥ : 2022.10.08
----------------------------------------------------------------------------------------------*/
class FileUtils
{
public:
	static Gvector<BYTE> ReadFile(const WCHAR* path);
	static GWString	ConvertUTF16(string str);
	//static WCHAR* ConvertWString(const char* str);
	//static int32 ConvertWCHAR(const char* str , WCHAR * des);
	//static char* ConvertCHAR(WCHAR* str);
};