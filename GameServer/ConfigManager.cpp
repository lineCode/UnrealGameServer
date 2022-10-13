#include "pch.h"
#include "ConfigManager.h"
#include "JsonParser.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include <fstream>

ConfigManager* ConfigManager::_Instance = nullptr;

/*---------------------------------------------------------------------------------------------
�̸�     : ConfigManager::GetInstance
�뵵     : ConfigManager �ν��Ͻ��� ��ȯ���ִ� �Լ�
������   : �̹α�
������¥ : 2022.10.2
----------------------------------------------------------------------------------------------*/
ConfigManager* ConfigManager::GetInstacnce()
{
	if (_Instance == nullptr)
		_Instance = new ConfigManager();

	return _Instance;
}

/*---------------------------------------------------------------------------------------------
�̸�     : ConfigManager::DestroyInstance
�뵵     : ConfigManager �ν��Ͻ��� �Ҹ����ִ� �Լ�
������   : �̹α�
������¥ : 2022.10.2
----------------------------------------------------------------------------------------------*/
void ConfigManager::DestroyInstance()
{
	if (_Instance != nullptr)
		delete _Instance;

	_Instance = nullptr;
}

/*---------------------------------------------------------------------------------------------
�̸�     : ConfigManager::LoadConfig
�뵵     : Config���� ��θ� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.12
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
