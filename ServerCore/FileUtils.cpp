#include "pch.h"
#include "FileUtils.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

/*---------------------------------------------------------------------------------------------
�̸�     : FileUtils::ReadFile
�뵵     : ��� �Է½� ������ �о�� �� ��ȯ�ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.08
----------------------------------------------------------------------------------------------*/
Gvector<BYTE> FileUtils::ReadFile(const WCHAR* path)
{
	Gvector<BYTE> ret;

	fs::path filePath{ path };

	const uint32 fileSize = static_cast<uint32>(fs::file_size(filePath));
	ret.resize(fileSize);

	basic_ifstream<BYTE> inputStream{ filePath };
	inputStream.read(&ret[0], fileSize);

	return ret;
}

/*---------------------------------------------------------------------------------------------
�̸�     : FileUtils::ConvertUTF16
�뵵     : ������ UTF-16���� ��ȯ�ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.08
----------------------------------------------------------------------------------------------*/
GWString FileUtils::ConvertUTF16(string str)
{
	const int32 srcLen = static_cast<int32>(str.size());

	GWString ret;
	if (srcLen == 0)
		return ret;

	const int32 retLen = ::MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<char*>(&str[0]), srcLen, NULL, 0);
	ret.resize(retLen);
	::MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<char*>(&str[0]), srcLen, &ret[0], retLen);

	return ret;
}

///*-----------------------------------------------------------------------------
//�̸�     : FileUtils::ConvertWString
//�뵵     : char���¸� Wstring���� ��ȯ���ִ� �Լ�
//������   : �̹α�
//������¥ : 2022.10.12
//------------------------------------------------------------------------------*/
//WCHAR* FileUtils::ConvertWString(const char* str)
//{
//	wchar_t* pStr;
//
//	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
//
//	pStr = new WCHAR[strSize];
//
//	MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, pStr, strSize);
//	return pStr;
//}
//
///*-----------------------------------------------------------------------------
//�̸�     : FileUtils::ConvertWCHAR
//�뵵     : char���¸� WCHAR������ ��ȯ���ִ� �Լ�
//           ����ִ� ������ ���̵� ��ȯ
//������   : �̹α�
//������¥ : 2022.10.12
//------------------------------------------------------------------------------*/
//int32 FileUtils::ConvertWCHAR(const char* str, WCHAR* des)
//{
//	wstring src = ConvertWString(str);
//
//	wcscpy_s(des, src.size() * 2 + 1, src.c_str());
//
//	return src.size()+1;
//}
//
///*-----------------------------------------------------------------------------
//�̸�     : FileUtils::ConvertCHAR
//�뵵     : WCHAR���¸� CHAR������ ��ȯ���ִ� �Լ�
//������   : �̹α�
//������¥ : 2022.10.16
//------------------------------------------------------------------------------*/
//char * FileUtils::ConvertCHAR(WCHAR* str)
//{
//	//��ȯ�� char* ���� ����
//	char* pStr;
//
//	//�Է¹��� wchar_t ������ ���̸� ����
//	int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
//	//char* �޸� �Ҵ�
//	pStr = new char[strSize];
//
//	//�� ��ȯ 
//	WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);
//	return pStr;
//}
