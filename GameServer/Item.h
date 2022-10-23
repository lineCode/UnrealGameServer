#pragma once
#include "Protocol.pb.h"

/*---------------------------------------------------------------------------------------------
이름     : Item
용도     : 서버에서 플레이어 아이템 저장을 위한 객체
수정자   : 이민규
수정날짜 : 2022.10.19
----------------------------------------------------------------------------------------------*/
class Item
{
public:
	Item(Protocol::ItemType type) :_itemtype(type) {}

	GetSetMaker(Protocol::ItemInfo,  info, _info);
	GetSetMaker(Protocol::ItemType, ItemType, _itemtype);
	GetSetMaker(bool, Stackable, _stackable);
	ProtoGetSetMaker(int32,  ItemDbID, _info.itemdbid , _info.set_itemdbid);
	ProtoGetSetMaker(int32,  ItemGameID , _info.itemgameid , _info.set_itemgameid);
	ProtoGetSetMaker(int32,  Count, _info.count , _info.set_count);
	ProtoGetSetMaker(int32,  Slot, _info.slot , _info.set_slot);

	static Item* MakeItem(int32 dbid , int32 gameid , int32 count , int32 slot);

protected:
	Protocol::ItemInfo _info;
	Protocol::ItemType _itemtype;
	bool _stackable;
};

/*---------------------------------------------------------------------------------------------
이름     : Weapon
용도     : 서버에서 플레이어 무기 아이템 저장을 위한 객체
수정자   : 이민규
수정날짜 : 2022.10.19
----------------------------------------------------------------------------------------------*/
class Weapon : public Item
{
public:
	Weapon(int32 gameid);
	GetSetMaker(Protocol::WeaponType, WeaponType, _weapontype);
	GetSetMaker(int32, Damage, _Damage);

private:
	void Init(int32 gameid);

private:
	Protocol::WeaponType _weapontype;
	int32 _Damage;
};

/*---------------------------------------------------------------------------------------------
이름     : Armor
용도     : 서버에서 플레이어 방어구 아이템 저장을 위한 객체
수정자   : 이민규
수정날짜 : 2022.10.19
----------------------------------------------------------------------------------------------*/
class Armor : public Item
{
public:
	Armor(int32 gameid);
	GetSetMaker(Protocol::ArmorType, ArmorType, _armortype);
	GetSetMaker(int32, Defence, _Defence);

private:
	void Init(int32 gameid);

private:
	Protocol::ArmorType _armortype;
	int32 _Defence;
};

/*---------------------------------------------------------------------------------------------
이름     : Consumable
용도     : 서버에서 플레이어 소비구 아이템 저장을 위한 객체
수정자   : 이민규
수정날짜 : 2022.10.19
----------------------------------------------------------------------------------------------*/
class Consumable : public Item
{
public:
	Consumable(int32 gameid);
	GetSetMaker(Protocol::ConsumableType, ConsumableType, _consumabletype);
	GetSetMaker(int32, MaxCount, _maxcount);

private:
	void Init(int32 gameid);

private:
	Protocol::ConsumableType _consumabletype;
	int32 _maxcount;
};