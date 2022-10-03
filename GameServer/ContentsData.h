#pragma once
#include "rapidjson/document.h"
#include "Protocol.pb.h"

 /*---------------------------------------------------------------------------------------------
 이름     : ILoaderData
 용도     : Json 데이터를 직렬화 후 GHashMap 만들기 위해 만든 Interface 클래스
 수정자   : 이민규
 수정날짜 : 2022.10.1
 ----------------------------------------------------------------------------------------------*/
 template<typename Key, typename Value>
 class ILoaderData
 {
 public:
 	virtual ~ILoaderData() = default;
 	virtual GhashMap<Key, Value> MakeGHashMap() = 0;
 };

#pragma region PlayerStat

 /*---------------------------------------------------------------------------------------------
 이름     : playerStatData
 용도     : Json 오브젝트를 PlayerStat에 맞게 데이터 직렬화 후 GHashMap을 만들어 반환하기 위한 객체
 수정자   : 이민규
 수정날짜 : 2022.10.3
 ----------------------------------------------------------------------------------------------*/
 class playerStatData : public ILoaderData<int32, Protocol::StatInfo>
 {
 public:
 	playerStatData(rapidjson::Document & document) : _Document(document) {}

 	/*---------------------------------------------------------------------------------------------
 	이름     : playerStatData::MakeTMap
 	용도     : Json 오브젝트를 PlayerStat에 맞게 데이터 직렬화 후 GHashMap을 만들어 반환하는 함수
 	수정자   : 이민규
 	수정날짜 : 2022.10.3
 	----------------------------------------------------------------------------------------------*/
 	GhashMap<int32, Protocol::StatInfo> MakeGHashMap() override
 	{
 		GhashMap<int32, Protocol::StatInfo> hashmap;

        const rapidjson::Value& JsonValues = _Document["playerstats"];

 		for (auto & JsonValue : JsonValues.GetArray())
 		{
            Protocol::StatInfo statdata{};
            statdata.set_level(JsonValue["level"].GetInt());
            statdata.set_maxhp(JsonValue["hp"].GetInt());
            statdata.set_hp(JsonValue["hp"].GetInt());
            statdata.set_damage(JsonValue["damage"].GetInt());
            statdata.set_speed(JsonValue["speed"].GetFloat());
            statdata.set_totalexp(JsonValue["exp"].GetInt());

 			hashmap.insert({statdata.level() , statdata});
 		}

 		return hashmap;
 	}

 private:
 	rapidjson::Document & _Document;
 };
 #pragma endregion

#pragma region SkillStat

 /*---------------------------------------------------------------------------------------------
 이름     : SkillStat
 용도     : Json파일에 있는 SkillStat을 저장하기 위한 구조체
 수정자   : 이민규
 수정날짜 : 2022.10.2
 ----------------------------------------------------------------------------------------------*/
 struct ProjectileStat
 {
 public:
     string name;
     float speed;
     int damage;
     string prefab;

 };

 struct SkillStat
 {
 public:
     int32 id;
     string name;
     float cooldown;
     int damage;
     Protocol::SkillType skilltype;
     //ProjectileStat projectilestat;
 };



 /*---------------------------------------------------------------------------------------------
 이름     : SkillStatData
 용도     : Json 오브젝트를 SkillStat에 맞게 데이터 직렬화 후 GHashMap을 만들어 반환하기 위한 객체
 수정자   : 이민규
 수정날짜 : 2022.10.2
 ----------------------------------------------------------------------------------------------*/
 class SkillStatData : public ILoaderData<int32, SkillStat>
 {
 public:
     SkillStatData(rapidjson::Document& document) : _Document(document) {}

     /*---------------------------------------------------------------------------------------------
     이름     : SkillStatData::MakeTMap
     용도     : Json 오브젝트를 SkillStat에 맞게 데이터 직렬화 후 GHashMap을 만들어 반환하는 함수
     수정자   : 이민규
     수정날짜 : 2022.10.2
     ----------------------------------------------------------------------------------------------*/
     GhashMap<int32, SkillStat> MakeGHashMap() override
     {
         GhashMap<int32, SkillStat> hashmap;

         const rapidjson::Value& JsonValues = _Document["skills"];

         for (auto& JsonValue : JsonValues.GetArray())
         {
             SkillStat statdata{};
             statdata.id = JsonValue["id"].GetInt();
             statdata.name = JsonValue["name"].GetString();
             statdata.cooldown = JsonValue["cooldown"].GetFloat();
             statdata.damage = JsonValue["damage"].GetInt();
             statdata.skilltype = static_cast<Protocol::SkillType>(JsonValue["skillType"].GetInt());
             hashmap.insert({ statdata.id , statdata });
         }

         return hashmap;
     }

 private:
     rapidjson::Document& _Document;
 };
#pragma endregion 