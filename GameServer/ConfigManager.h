#pragma once

struct ServerConfig
{
public:
	string _DataPath;
	GWString _DBLogin;
	GWString _DBPath;
};

/*---------------------------------------------------------------------------------------------
 이름     : ConfigManager
 용도     : 싱글턴 패턴으로 json Config파일들을 읽어와 관리하는 객체
 수정자   : 이민규
 수정날짜 : 2022.10.2
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


