#pragma once
#include "rapidjson/document.h"

/*-----------------------------------------------------------------------------
이름     : JsonObject
용도     : JsonObject를 관리 및 변경을 편하게 하기 위한 객체
수정자   : 이민규
수정날짜 : 2022.10.12
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
이름     : JsonParser
용도     : Json을 읽어오는 객체
수정자   : 이민규
수정날짜 : 2022.10.12
------------------------------------------------------------------------------*/
class JsonParser
{
public :
	void ParseJson(const WCHAR* path , rapidjson::Document & document);
	const WCHAR* ConverWString(const char* str);
};

