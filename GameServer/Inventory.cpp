#include "pch.h"
#include "Inventory.h"

#include "Item.h"

Inventory::Inventory()
{
	for (int32 i = 0; i < 20; i++)
		slots[i] = true;
}

Inventory::~Inventory()
{
	for (auto& item : _Items)
		Gdelete(item.second);
}

/*---------------------------------------------------------------------------------------------
이름     : Inventory::Add
용도     : 인벤토리에 아이템을 추가하는 함수
수정자   : 이민규
수정날짜 : 2022.10.19
----------------------------------------------------------------------------------------------*/
void Inventory::Add(Item* item)
{
	slots[item->GetSlot()] = false;
	_Items.insert({ item->GetItemDbID() , item });
}

/*---------------------------------------------------------------------------------------------
이름     : Inventory::Erase
용도     : 인벤토리에서 아이템을 제거하는 함수
수정자   : 이민규
수정날짜 : 2022.10.19
----------------------------------------------------------------------------------------------*/
void Inventory::Erase(Item* item)
{
	slots[item->GetSlot()] = true;
	_Items.erase(item->GetItemDbID());
}

/*---------------------------------------------------------------------------------------------
이름     : Inventory::Find
용도     : 인벤토리에 있는 아이템을 가져오는 함수
           없을 경우 nullptr 반환
수정자   : 이민규
수정날짜 : 2022.10.19
----------------------------------------------------------------------------------------------*/
Item* Inventory::Find(int32 itemdbid)
{
	auto item = _Items.find(itemdbid);

	if (item == _Items.end())
		return nullptr;
	
	return item->second;
}

/*---------------------------------------------------------------------------------------------
이름     : Inventory::FindCondition
용도     : 인벤토리에 있는 아이템을 조건식에 따라 가져오는 함수
           없을 경우 nullptr 반환
수정자   : 이민규
수정날짜 : 2022.10.19
----------------------------------------------------------------------------------------------*/
Item* Inventory::FindCondition(function<bool(Item*)> condition)
{
	for (auto& item : _Items)
		if (condition(item.second))
			return item.second;

	return nullptr;
}

/*---------------------------------------------------------------------------------------------
이름     : Inventory::GetEmptySlot
용도     : 인벤토리의 슬롯중 빈슬롯을 알려주는 함수
           Full 일 경우 -1 반환
수정자   : 이민규
수정날짜 : 2022.10.24
----------------------------------------------------------------------------------------------*/
int32 Inventory::GetEmptySlot()
{
	for(int i = 0; i < 20; i++)
	{
		if (slots[i])
			return i;
	}

	return -1;
}
