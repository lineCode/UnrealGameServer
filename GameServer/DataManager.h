#pragma once
#include <fstream>
#include <format>

#include "ConfigManager.h"
#include "ContentsData.h"
#include "rapidjson/istreamwrapper.h"

/*---------------------------------------------------------------------------------------------
 이름     : DataManager
 용도     : 싱글턴 패턴으로 Json파일을 읽어와 데이터를 로드하고 저장하는 객체
 수정자   : 이민규
 수정날짜 : 2022.10.2
 ----------------------------------------------------------------------------------------------*/
 class DataManager
 {
 public:
 	static DataManager* GetInstacnce();
 	static void DestroyInstance();
 	void LodaData();

 	playerStat GetPlayerStatData(int32 level) { return _PlayerStatDatas[level]; }
 	SkillStat GetSkillStatData(int32 id) { return _SkillStatDatas[id]; }

private:
    template <class T, class Key, class Value>
    requires std::derived_from<T, ILoaderData<Key, Value>>
    T LoadJson(string jsonname);

 private:
 	static DataManager * _Instance;
    rapidjson::Document _document;

 	GhashMap<int32, playerStat> _PlayerStatDatas;
 	GhashMap<int32, SkillStat> _SkillStatDatas;
 };

 /*---------------------------------------------------------------------------------------------
 이름     : DataManager::LoadJson
 용도     : Json파일을 읽어와 데이터를 로드하고 StatData를 반환하는 함수
 수정자   : 이민규
 수정날짜 : 2022.10.2
 ----------------------------------------------------------------------------------------------*/
 template<typename T , typename Key , typename Value>
 requires std::derived_from<T , ILoaderData<Key , Value>>
T DataManager::LoadJson(const string jsonname)
 {
     ifstream ifs(ConfigManager::GetInstacnce()->GetServerConfig()._DataPath + jsonname + ".json");

     if (ifs.is_open() == false)
         cout << "[DATAMANAGER] : json open Error" << endl;

     rapidjson::IStreamWrapper jsonreader(ifs);
     _document.ParseStream(jsonreader);
     assert(_document.IsObject());

     T data(_document);
     return data;
 }