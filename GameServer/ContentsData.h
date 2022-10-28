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
 이름     : playerStatLoader
 용도     : Json 오브젝트를 PlayerStat에 맞게 데이터 직렬화 후 GHashMap을 만들어 반환하기 위한 객체
 수정자   : 이민규
 수정날짜 : 2022.10.3
 ----------------------------------------------------------------------------------------------*/
 class playerStatLoader : public ILoaderData<int32, Protocol::StatInfo>
 {
 public:
 	playerStatLoader(rapidjson::Document & document) : _Document(document) {}

 	/*---------------------------------------------------------------------------------------------
 	이름     : playerStatLoader::MakeTMap
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
            statdata.set_attack(JsonValue["damage"].GetInt());
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
 이름     : SkillStatLoader
 용도     : Json 오브젝트를 SkillStat에 맞게 데이터 직렬화 후 GHashMap을 만들어 반환하기 위한 객체
 수정자   : 이민규
 수정날짜 : 2022.10.2
 ----------------------------------------------------------------------------------------------*/
 class SkillStatLoader : public ILoaderData<int32, SkillStat>
 {
 public:
     SkillStatLoader(rapidjson::Document& document) : _Document(document) {}

     /*---------------------------------------------------------------------------------------------
     이름     : SkillStatLoader::MakeTMap
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

#pragma region ItemData

struct ItemData
{
	int32 GameID;
    string name;
	Protocol::ItemType itemType;
};

struct WeaponData : public ItemData
{
    Protocol::WeaponType weaponType;
    int32 damage;
};

struct ArmorData : public ItemData
{
    Protocol::ArmorType ArmorType;
    int32 defence;
};
 
struct ConsumableData : public ItemData
{
    Protocol::ConsumableType ConsumableType;
    int32 maxcount;
};

/*---------------------------------------------------------------------------------------------
이름     : ItemLoader
용도     : Json 오브젝트를 Itemdata에 맞게 데이터 직렬화 후 GHashMap을 만들어 반환하기 위한 객체
수정자   : 이민규
수정날짜 : 2022.10.19
----------------------------------------------------------------------------------------------*/
 class ItemLoader : public ILoaderData<int32, ItemData*>
 {
 public:
     ItemLoader(rapidjson::Document& document) : _Document(document) {}

     Glist<WeaponData*> weapons;
     Glist<ArmorData*> armors;
     Glist<ConsumableData*> consumables;

     /*---------------------------------------------------------------------------------------------
     이름     : playerStatLoader::MakeTMap
     용도     : Json 오브젝트를 ItemData에 맞게 데이터 직렬화 후 GHashMap을 만들어 반환하는 함수
     수정자   : 이민규
     수정날짜 : 2022.10.19
     ----------------------------------------------------------------------------------------------*/
     GhashMap<int32, ItemData*> MakeGHashMap() override
     {
         GhashMap<int32, ItemData*> hashmap;

         const rapidjson::Value& JsonValues = _Document["weapons"];
         for (auto& JsonValue : JsonValues.GetArray())
         {
             WeaponData * weapondata = Gnew<WeaponData>();
             weapondata->GameID = JsonValue["gameid"].GetInt();
             weapondata->name = JsonValue["name"].GetString();
             weapondata->itemType = Protocol::ItemType::ITEM_TYPE_WEAPON;
             weapondata->damage = JsonValue["damage"].GetInt();

             string type = JsonValue["weaponType"].GetString();
             if(type.compare("sword") == 0)
                 weapondata->weaponType = Protocol::WEAPON_TYPE_SWORD;
             else if(type.compare("bow") == 0)
                 weapondata->weaponType = Protocol::WEAPON_TYPE_BOW;
             else
                 weapondata->weaponType = Protocol::WEAPON_TYPE_NONE;

             weapons.push_back(weapondata);
         }

         const rapidjson::Value& JsonValues2 = _Document["armors"];
         for (auto& JsonValue : JsonValues2.GetArray())
         {
             ArmorData * armordata = Gnew<ArmorData>();
             armordata->GameID = JsonValue["gameid"].GetInt();
             armordata->name = JsonValue["name"].GetString();
             armordata->itemType = Protocol::ItemType::ITEM_TYPE_ARMOR;
             armordata->defence = JsonValue["defence"].GetInt();

             string type = JsonValue["armorType"].GetString();
             if (type.compare("helmet") == 0)
                 armordata->ArmorType = Protocol::ARMOR_TYPE_HELMET;
             else if (type.compare("armor") == 0)
                 armordata->ArmorType = Protocol::ARMOR_TYPE_ARMOR;
             else if (type.compare("boots") == 0)
                 armordata->ArmorType = Protocol::ARMOR_TYPE_BOOTS;
             else
                 armordata->ArmorType = Protocol::ARMOR_TYPE_NONE;

             armors.push_back(armordata);
         }

         const rapidjson::Value& JsonValues3 = _Document["consumables"];
         for (auto& JsonValue : JsonValues3.GetArray())
         {
             ConsumableData * consumabledata = Gnew<ConsumableData>();
             consumabledata->GameID = JsonValue["gameid"].GetInt();
             consumabledata->name = JsonValue["name"].GetString();
             consumabledata->itemType = Protocol::ItemType::ITEM_TYPE_CONSUMABLE;
             consumabledata->maxcount = JsonValue["maxcount"].GetInt();

             string type = JsonValue["consumableType"].GetString();
             if (type.compare("potion") == 0)
                 consumabledata->ConsumableType = Protocol::CONSUMABLE_TYPE_POTION;
             else
                 consumabledata->ConsumableType = Protocol::CONSUMABLE_TYPE_NONE;

             consumables.push_back(consumabledata);
         }

        for(auto weapon : weapons)
            hashmap.insert({ weapon->GameID,  weapon });

        for (auto armor : armors)
            hashmap.insert({ armor->GameID, armor });

        for (auto consumable : consumables)
            hashmap.insert({ consumable->GameID, consumable });
        
         return hashmap;
     }

 private:
     rapidjson::Document& _Document;
 };
#pragma endregion

#pragma region MonsterData

struct RewardData
{
    int probability; // 100분율
    int itemid;
    int count;
};

struct MonsterData
{
    int id;
    string name;
    Protocol::StatInfo stat;
    Glist<RewardData*> rewards;
};

 /*---------------------------------------------------------------------------------------------
 이름     : MonsterLoader
 용도     : Json 오브젝트를 Monster에 맞게 데이터 직렬화 후 GHashMap을 만들어 반환하기 위한 객체
 수정자   : 이민규
 수정날짜 : 2022.10.24
 ----------------------------------------------------------------------------------------------*/
 class MonsterLoader : public ILoaderData<int32, MonsterData*>
 {
 public:
     MonsterLoader(rapidjson::Document& document) : _Document(document) {}

     /*---------------------------------------------------------------------------------------------
     이름     : playerStatLoader::MakeTMap
     용도     : Json 오브젝트를 Monster에 맞게 데이터 직렬화 후 GHashMap을 만들어 반환하는 함수
     수정자   : 이민규
     수정날짜 : 2022.10.24
     ----------------------------------------------------------------------------------------------*/
     GhashMap<int32, MonsterData*> MakeGHashMap() override
     {
         GhashMap<int32, MonsterData*> hashmap;

         const rapidjson::Value& JsonValues = _Document["monsters"];

         for (auto& JsonValue : JsonValues.GetArray())
         {
             MonsterData* monster = Gnew<MonsterData>();
             monster->id = JsonValue["id"].GetInt();
             monster->name = JsonValue["name"].GetString();

             const rapidjson::Value& JsonStat = JsonValue["stat"];
             monster->stat.set_level(JsonStat["level"].GetInt());
             monster->stat.set_maxhp(JsonStat["hp"].GetInt());
             monster->stat.set_hp(JsonStat["hp"].GetInt());
             monster->stat.set_attack(JsonStat["damage"].GetInt());
             monster->stat.set_speed(JsonStat["speed"].GetInt());
             monster->stat.set_totalexp(JsonStat["exp"].GetInt());

             const rapidjson::Value& JsonRewards = JsonValue["rewards"];
             for (auto& reward : JsonRewards.GetArray())
             {
             	RewardData * rewarddata = Gnew<RewardData>();
                rewarddata->probability = reward["probability"].GetInt();
                rewarddata->itemid = reward["itemid"].GetInt();
                rewarddata->count = reward["count"].GetInt();

                monster->rewards.push_back(rewarddata);
             }

             hashmap.insert({ monster->id , monster });
         }

         return hashmap;
     }

 private:
     rapidjson::Document& _Document;
 };

#pragma endregion