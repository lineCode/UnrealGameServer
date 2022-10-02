#include "pch.h"
#include "ConfigManager.h"
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

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
������¥ : 2022.10.2
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
