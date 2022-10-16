#include "pch.h"
#include "FileUtils.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

/*---------------------------------------------------------------------------------------------
이름     : FileUtils::ReadFile
용도     : 경로 입력시 파일을 읽어온 후 반환하는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
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
이름     : FileUtils::ConvertUTF16
용도     : 파일을 UTF-16으로 변환하는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
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
//이름     : FileUtils::ConvertWString
//용도     : char형태를 Wstring으로 변환해주는 함수
//수정자   : 이민규
//수정날짜 : 2022.10.12
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
//이름     : FileUtils::ConvertWCHAR
//용도     : char형태를 WCHAR형으로 변환해주는 함수
//           들고있는 문자의 길이도 반환
//수정자   : 이민규
//수정날짜 : 2022.10.12
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
//이름     : FileUtils::ConvertCHAR
//용도     : WCHAR형태를 CHAR형으로 변환해주는 함수
//수정자   : 이민규
//수정날짜 : 2022.10.16
//------------------------------------------------------------------------------*/
//char * FileUtils::ConvertCHAR(WCHAR* str)
//{
//	//반환할 char* 변수 선언
//	char* pStr;
//
//	//입력받은 wchar_t 변수의 길이를 구함
//	int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
//	//char* 메모리 할당
//	pStr = new char[strSize];
//
//	//형 변환 
//	WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);
//	return pStr;
//}
