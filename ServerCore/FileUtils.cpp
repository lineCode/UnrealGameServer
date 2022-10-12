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
�̸�     : FileUtils::Convert
�뵵     : ������ UTF-16���� ��ȯ�ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.08
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
