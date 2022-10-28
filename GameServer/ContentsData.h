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
 �̸�     : playerStatLoader
 �뵵     : Json ������Ʈ�� PlayerStat�� �°� ������ ����ȭ �� GHashMap�� ����� ��ȯ�ϱ� ���� ��ü
 ������   : �̹α�
 ������¥ : 2022.10.3
 ----------------------------------------------------------------------------------------------*/
 class playerStatLoader : public ILoaderData<int32, Protocol::StatInfo>
 {
 public:
 	playerStatLoader(rapidjson::Document & document) : _Document(document) {}

 	/*---------------------------------------------------------------------------------------------
 	�̸�     : playerStatLoader::MakeTMap
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
 �̸�     : SkillStatLoader
 �뵵     : Json ������Ʈ�� SkillStat�� �°� ������ ����ȭ �� GHashMap�� ����� ��ȯ�ϱ� ���� ��ü
 ������   : �̹α�
 ������¥ : 2022.10.2
 ----------------------------------------------------------------------------------------------*/
 class SkillStatLoader : public ILoaderData<int32, SkillStat>
 {
 public:
     SkillStatLoader(rapidjson::Document& document) : _Document(document) {}

     /*---------------------------------------------------------------------------------------------
     �̸�     : SkillStatLoader::MakeTMap
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
�̸�     : ItemLoader
�뵵     : Json ������Ʈ�� Itemdata�� �°� ������ ����ȭ �� GHashMap�� ����� ��ȯ�ϱ� ���� ��ü
������   : �̹α�
������¥ : 2022.10.19
----------------------------------------------------------------------------------------------*/
 class ItemLoader : public ILoaderData<int32, ItemData*>
 {
 public:
     ItemLoader(rapidjson::Document& document) : _Document(document) {}

     Glist<WeaponData*> weapons;
     Glist<ArmorData*> armors;
     Glist<ConsumableData*> consumables;

     /*---------------------------------------------------------------------------------------------
     �̸�     : playerStatLoader::MakeTMap
     �뵵     : Json ������Ʈ�� ItemData�� �°� ������ ����ȭ �� GHashMap�� ����� ��ȯ�ϴ� �Լ�
     ������   : �̹α�
     ������¥ : 2022.10.19
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
    int probability; // 100����
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
 �̸�     : MonsterLoader
 �뵵     : Json ������Ʈ�� Monster�� �°� ������ ����ȭ �� GHashMap�� ����� ��ȯ�ϱ� ���� ��ü
 ������   : �̹α�
 ������¥ : 2022.10.24
 ----------------------------------------------------------------------------------------------*/
 class MonsterLoader : public ILoaderData<int32, MonsterData*>
 {
 public:
     MonsterLoader(rapidjson::Document& document) : _Document(document) {}

     /*---------------------------------------------------------------------------------------------
     �̸�     : playerStatLoader::MakeTMap
     �뵵     : Json ������Ʈ�� Monster�� �°� ������ ����ȭ �� GHashMap�� ����� ��ȯ�ϴ� �Լ�
     ������   : �̹α�
     ������¥ : 2022.10.24
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