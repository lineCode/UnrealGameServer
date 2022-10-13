#include "pch.h"
#include "JsonParser.h"
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

/*-----------------------------------------------------------------------------
�̸�     : JsonObject::GetWString
�뵵     : JsonObject�� char���� wchar������ ���� �� ��ȯ���ִ� �Լ�
������   : �̹α�
������¥ : 2022.10.12
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
�̸�     : JsonObject::GetArrayGvector
�뵵     : �迭 ������ Object�� Gvector���·� �������� �Լ�
������   : �̹α�
������¥ : 2022.10.12
------------------------------------------------------------------------------*/
Gvector<JsonObject> JsonObject::GetArrayGvector(const char* name)
{
    Gvector<JsonObject> objects;

    for (JsonObject object : _object[name].GetArray())
        objects.push_back(object);

    return objects;
}

/*-----------------------------------------------------------------------------
�̸�     : JsonParser::ParseJson
�뵵     : ����� Json�� �а� �Ľ����ִ� ��ü
������   : �̹α�
������¥ : 2022.10.12
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
�̸�     : JsonParser::ConvertWString
�뵵     : char���¸� Wstring���� ��ȯ���ִ� �Լ�
������   : �̹α�
������¥ : 2022.10.12
------------------------------------------------------------------------------*/
const WCHAR* JsonParser::ConvertWString(const char * str)
{
    wchar_t* pStr;

    int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);

    pStr = new WCHAR[strSize];

    MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, pStr, strSize);
    return pStr;
}
