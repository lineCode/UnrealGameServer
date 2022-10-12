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
이름     : FileUtils::Convert
용도     : 파일을 UTF-16으로 변환하는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
----------------------------------------------------------------------------------------------*/
GWString FileUtils::Convert(string str)
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
