#pragma once
#include "rapidjson/document.h"
#include "Protocol.pb.h"

 /*---------------------------------------------------------------------------------------------
 �̸�     : ILoaderData
 �뵵     : Json �����͸� ����ȭ �� GHashMap ����� ���� ���� Interface Ŭ����
 ������   : �̹α�
 ������¥ : 2022.10.1
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
 �̸�     : playerStatData
 �뵵     : Json ������Ʈ�� PlayerStat�� �°� ������ ����ȭ �� GHashMap�� ����� ��ȯ�ϱ� ���� ��ü
 ������   : �̹α�
 ������¥ : 2022.10.3
 ----------------------------------------------------------------------------------------------*/
 class playerStatData : public ILoaderData<int32, Protocol::StatInfo>
 {
 public:
 	playerStatData(rapidjson::Document & document) : _Document(document) {}

 	/*---------------------------------------------------------------------------------------------
 	�̸�     : playerStatData::MakeTMap
 	�뵵     : Json ������Ʈ�� PlayerStat�� �°� ������ ����ȭ �� GHashMap�� ����� ��ȯ�ϴ� �Լ�
 	������   : �̹α�
 	������¥ : 2022.10.3
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
 �̸�     : SkillStat
 �뵵     : Json���Ͽ� �ִ� SkillStat�� �����ϱ� ���� ����ü
 ������   : �̹α�
 ������¥ : 2022.10.2
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
 �̸�     : SkillStatData
 �뵵     : Json ������Ʈ�� SkillStat�� �°� ������ ����ȭ �� GHashMap�� ����� ��ȯ�ϱ� ���� ��ü
 ������   : �̹α�
 ������¥ : 2022.10.2
 ----------------------------------------------------------------------------------------------*/
 class SkillStatData : public ILoaderData<int32, SkillStat>
 {
 public:
     SkillStatData(rapidjson::Document& document) : _Document(document) {}

     /*---------------------------------------------------------------------------------------------
     �̸�     : SkillStatData::MakeTMap
     �뵵     : Json ������Ʈ�� SkillStat�� �°� ������ ����ȭ �� GHashMap�� ����� ��ȯ�ϴ� �Լ�
     ������   : �̹α�
     ������¥ : 2022.10.2
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