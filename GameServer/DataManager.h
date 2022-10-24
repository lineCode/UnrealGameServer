#pragma once

#include "ConfigManager.h"
#include "ContentsData.h"
#include "JsonParser.h"
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

    Protocol::StatInfo GetPlayerStatData(int32 level) { return _PlayerStatDatas[level]; }
 	SkillStat GetSkillStatData(int32 id) { return _SkillStatDatas[id]; }
 	ItemData * GetItemData(int32 gameid) {if (_ItemDatas.contains(gameid) == false)return nullptr;return _ItemDatas[gameid];}
    MonsterData* GetMonsterData(int32 monsterid) {if (_MonsterDatas.contains(monsterid) == false)return nullptr;return _MonsterDatas[monsterid];}

private:
    template <class T, class Key, class Value>
    requires std::derived_from<T, ILoaderData<Key, Value>>
    T LoadJson(string jsonname);

 private:
 	static DataManager * _Instance;
    rapidjson::Document _document;

 	GhashMap<int32, Protocol::StatInfo> _PlayerStatDatas;
 	GhashMap<int32, SkillStat> _SkillStatDatas;
 	GhashMap<int32, ItemData*> _ItemDatas;
 	GhashMap<int32, MonsterData*> _MonsterDatas;
 };

 /*---------------------------------------------------------------------------------------------
 이름     : DataManager::LoadJson
 용도     : Json파일을 읽어와 데이터를 로드하고 StatData를 반환하는 함수
 수정자   : 이민규
 수정날짜 : 2022.10.12
 ----------------------------------------------------------------------------------------------*/
 template<typename T , typename Key , typename Value>
 requires std::derived_from<T , ILoaderData<Key , Value>>
T DataManager::LoadJson(const string jsonname)
 {
    JsonParser parser;
    string str = ConfigManager::GetInstacnce()->GetServerConfig()._DataPath + jsonname + ".json";
    parser.ParseJson(parser.ConvertWString(str.c_str()) , _document);

    assert(_document.IsObject());

    T data(_document);
    return data;
 }