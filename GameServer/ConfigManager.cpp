#include "pch.h"
#include "ConfigManager.h"
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

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
수정날짜 : 2022.10.2
----------------------------------------------------------------------------------------------*/
void ConfigManager::LoadConfig()
{
	ifstream ifs("../Binary/Debug/config.json");

	if (ifs.is_open() == false)
		cout << "[LoadConfig] : json open Error" << endl;

	rapidjson::IStreamWrapper jsonreader(ifs);
	rapidjson::Document document;

	document.ParseStream(jsonreader);

	_config._DataPath = document["datapath"].GetString();
}
