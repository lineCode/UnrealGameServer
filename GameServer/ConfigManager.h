#pragma once

struct ServerConfig
{
public:
	string _DataPath;
	GWString _DBLogin;
	GWString _DBPath;
};

/*---------------------------------------------------------------------------------------------
 �̸�     : ConfigManager
 �뵵     : �̱��� �������� json Config���ϵ��� �о�� �����ϴ� ��ü
 ������   : �̹α�
 ������¥ : 2022.10.2
 ----------------------------------------------------------------------------------------------*/
class ConfigManager
{
public:
	static ConfigManager* GetInstacnce();
	static void DestroyInstance();

	void LoadConfig();
	ServerConfig GetServerConfig() { return _config; }
private:
	static ConfigManager * _Instance;
	ServerConfig _config;
};


