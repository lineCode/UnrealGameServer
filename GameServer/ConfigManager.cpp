#include "pch.h"
#include "ConfigManager.h"
#include "JsonParser.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include <fstream>

ConfigManager* ConfigManager::_Instance = nullptr;

/*---------------------------------------------------------------------------------------------
이름     : ConfigManager::GetInstance
용도     : ConfigManager 인스턴스를 반환해주는 함수
수정자   : 이민규
수정날짜 : 2022.10.2
----------------------------------------------------------------------------------------------*/
ConfigManager* ConfigManager::GetInstacnce()
{
	if (_Instance == nullptr)
		_Instance = new ConfigManager();

	return _Instance;
}

/*---------------------------------------------------------------------------------------------
이름     : ConfigManager::DestroyInstance
용도     : ConfigManager 인스턴스를 소멸해주는 함수
수정자   : 이민규
수정날짜 : 2022.10.2
----------------------------------------------------------------------------------------------*/
void ConfigManager::DestroyInstance()
{
	if (_Instance != nullptr)
		delete _Instance;

	_Instance = nullptr;
}

/*---------------------------------------------------------------------------------------------
이름     : ConfigManager::LoadConfig
용도     : Config들의 경로를 저장하는 함수
수정자   : 이민규
수정날짜 : 2022.10.12
----------------------------------------------------------------------------------------------*/
void ConfigManager::LoadConfig()
{
	JsonParser parser;
	rapidjson::Document document;

	parser.ParseJson(L"../Binary/Debug/config.json", document);

	_config._DataPath =  document["datapath"].GetString();
	_config._DBLogin = parser.ConvertWString(document["DBLogin"].GetString());
	_config._DBPath = parser.ConvertWString(document["DBPath"].GetString());
}
