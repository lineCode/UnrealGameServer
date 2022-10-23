#include "pch.h"
#include "Item.h"
#include "DataManager.h"

/*---------------------------------------------------------------------------------------------
이름     : Item::MakeItem
용도     : 아이템 번호에 맞게 아이템을 생성하고 세팅해주는 함수
수정자   : 이민규
수정날짜 : 2022.10.23
----------------------------------------------------------------------------------------------*/
Item* Item::MakeItem(int32 dbid, int32 gameid , int32 count , int32 slot)
{
	auto * data = DataManager::GetInstacnce()->GetItemData(gameid);
	if (data == nullptr)
		return nullptr;

	Item* item = nullptr;

	switch (data->itemType)
	{
	case Protocol::ITEM_TYPE_WEAPON:
		{
			item = Gnew<Weapon>(gameid);
			break;
		}
	case Protocol::ITEM_TYPE_ARMOR:
		{
			item = Gnew<Armor>(gameid);
			break;
		}
	case Protocol::ITEM_TYPE_CONSUMABLE:
		{
			item = Gnew<Consumable>(gameid);
			break;
		}
	default: return nullptr;
	}

	if (item == nullptr)
		return nullptr;

	item->SetSlot(slot);
	item->SetItemDbID(dbid);
	item->SetCount(count);

	return item;
}

/*---------------------------------------------------------------------------------------------
이름     : Weapon
용도     : 서버에서 플레이어 무기 아이템 저장을 위한 객체
수정자   : 이민규
수정날짜 : 2022.10.19
----------------------------------------------------------------------------------------------*/
Weapon::Weapon(int32 gameid) : Item(Protocol::ItemType::ITEM_TYPE_WEAPON)
{
	Init(gameid);
}

void Weapon::Init(int32 gameid)
{
	ItemData * item = DataManager::GetInstacnce()->GetItemData(gameid);

	if (item == nullptr || item->itemType != Protocol::ITEM_TYPE_WEAPON)
		return;

	WeaponData* data = static_cast<WeaponData*>(item);
	SetItemGameID(data->GameID);
	SetCount(1);
	SetWeaponType(data->weaponType);
	SetDamage(data->damage);
	SetStackable(false);
}


/*---------------------------------------------------------------------------------------------
이름     : Armor
용도     : 서버에서 플레이어 방어구 아이템 저장을 위한 객체
수정자   : 이민규
수정날짜 : 2022.10.19
----------------------------------------------------------------------------------------------*/
Armor::Armor(int32 gameid) : Item(Protocol::ItemType::ITEM_TYPE_ARMOR)
{
	Init(gameid);
}

void Armor::Init(int32 gameid)
{
	ItemData* item = DataManager::GetInstacnce()->GetItemData(gameid);

	if (item == nullptr || item->itemType != Protocol::ITEM_TYPE_ARMOR)
		return;

	ArmorData* data = static_cast<ArmorData*>(item);
	SetItemGameID(data->GameID);
	SetCount(1);
	SetArmorType(data->ArmorType);
	SetDefence(data->defence);
	SetStackable(false);
}


/*---------------------------------------------------------------------------------------------
이름     : Consumable
용도     : 서버에서 플레이어 소비구 아이템 저장을 위한 객체
수정자   : 이민규
수정날짜 : 2022.10.19
----------------------------------------------------------------------------------------------*/
Consumable::Consumable(int32 gameid) : Item(Protocol::ItemType::ITEM_TYPE_CONSUMABLE)
{
	Init(gameid);
}

void Consumable::Init(int32 gameid)
{
	ItemData* item = DataManager::GetInstacnce()->GetItemData(gameid);

	if (item == nullptr || item->itemType != Protocol::ITEM_TYPE_CONSUMABLE)
		return;

	ConsumableData* data = static_cast<ConsumableData*>(item);
	SetItemGameID(data->GameID);
	SetCount(1);
	SetConsumableType(data->ConsumableType);
	SetMaxCount(data->maxcount);
	SetStackable(data->maxcount > 1);
}