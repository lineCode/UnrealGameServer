#pragma once

#include "ConfigManager.h"
#include "ContentsData.h"
#include "JsonParser.h"
#include "rapidjson/istreamwrapper.h"

/*---------------------------------------------------------------------------------------------
 �̸�     : DataManager
 �뵵     : �̱��� �������� Json������ �о�� �����͸� �ε��ϰ� �����ϴ� ��ü
 ������   : �̹α�
 ������¥ : 2022.10.2
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
 �̸�     : DataManager::LoadJson
 �뵵     : Json������ �о�� �����͸� �ε��ϰ� StatData�� ��ȯ�ϴ� �Լ�
 ������   : �̹α�
 ������¥ : 2022.10.12
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