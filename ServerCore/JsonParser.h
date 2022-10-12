#pragma once
#include "rapidjson/document.h"

/*-----------------------------------------------------------------------------
�̸�     : JsonObject
�뵵     : JsonObject�� ���� �� ������ ���ϰ� �ϱ� ���� ��ü
������   : �̹α�
������¥ : 2022.10.12
------------------------------------------------------------------------------*/
class JsonObject
{
public:
	JsonObject(const rapidjson::Value& object) : _object(object) {}

	const WCHAR* GetWString(const char * name);
	Gvector<JsonObject> GetArrayGvector(const char* name);

	const rapidjson::Value& _object;
};

/*-----------------------------------------------------------------------------
�̸�     : JsonParser
�뵵     : Json�� �о���� ��ü
������   : �̹α�
������¥ : 2022.10.12
------------------------------------------------------------------------------*/
class JsonParser
{
public :
	void ParseJson(const WCHAR* path , rapidjson::Document & document);
	const WCHAR* ConverWString(const char* str);
};

