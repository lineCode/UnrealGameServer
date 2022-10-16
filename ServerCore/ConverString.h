#pragma once

/*---------------------------------------------------------------------------------------------
�̸�     : ConverString
�뵵     : CHAR�� WCHAR���� ����ȯ�� �����ο� String
������   : �̹α�
������¥ : 2022.10.16
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

