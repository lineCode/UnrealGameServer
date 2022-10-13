#include "pch.h"
#include "JsonParser.h"
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

/*-----------------------------------------------------------------------------
이름     : JsonObject::GetWString
용도     : JsonObject의 char형을 wchar형으로 변경 후 반환해주는 함수
수정자   : 이민규
수정날짜 : 2022.10.12
------------------------------------------------------------------------------*/
const WCHAR* JsonObject::GetWString(const char* name)
{
    if (_object.HasMember(name) == false)
        return L"";

    const char* str = _object[name].GetString();

    wchar_t* pStr;

    int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);

    pStr = new WCHAR[strSize];

    MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, pStr, strSize);
    return pStr;
}

/*-----------------------------------------------------------------------------
이름     : JsonObject::GetArrayGvector
용도     : 배열 형태의 Object를 Gvector형태로 가져오는 함수
수정자   : 이민규
수정날짜 : 2022.10.12
------------------------------------------------------------------------------*/
Gvector<JsonObject> JsonObject::GetArrayGvector(const char* name)
{
    Gvector<JsonObject> objects;

    for (JsonObject object : _object[name].GetArray())
        objects.push_back(object);

    return objects;
}

/*-----------------------------------------------------------------------------
이름     : JsonParser::ParseJson
용도     : 경로의 Json을 읽고 파싱해주는 객체
수정자   : 이민규
수정날짜 : 2022.10.12
------------------------------------------------------------------------------*/
void JsonParser::ParseJson(const WCHAR* path , rapidjson::Document& document)
{
    ifstream ifs(path);

    if (ifs.is_open() == false)
        cout << "[JSONPARSER] : Json Not Open" << endl;

    rapidjson::IStreamWrapper jsonreader(ifs);
    document.ParseStream(jsonreader);

    assert(document.IsObject());
}

/*-----------------------------------------------------------------------------
이름     : JsonParser::ConvertWString
용도     : char형태를 Wstring으로 변환해주는 함수
수정자   : 이민규
수정날짜 : 2022.10.12
------------------------------------------------------------------------------*/
const WCHAR* JsonParser::ConvertWString(const char * str)
{
    wchar_t* pStr;

    int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);

    pStr = new WCHAR[strSize];

    MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, pStr, strSize);
    return pStr;
}
