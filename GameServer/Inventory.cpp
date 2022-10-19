#include "pch.h"
#include "Inventory.h"

#include "Item.h"

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
	_Items.insert({ item->GetItemDbID() , item });
}

/*---------------------------------------------------------------------------------------------
이름     : Inventory::GetItem
용도     : 인벤토리에 있는 아이템을 가져오는 함수
           없을 경우 nullptr 반환
수정자   : 이민규
수정날짜 : 2022.10.19
----------------------------------------------------------------------------------------------*/
Item* Inventory::GetItem(int itemid)
{
	auto item = _Items.find(itemid);

	if (item == _Items.end())
		return nullptr;
	
	return item->second;
}

/*---------------------------------------------------------------------------------------------
이름     : Inventory::Find
용도     : 인벤토리에 있는 아이템을 조건식에 따라 가져오는 함수
           없을 경우 nullptr 반환
수정자   : 이민규
수정날짜 : 2022.10.19
----------------------------------------------------------------------------------------------*/
Item* Inventory::Find(function<bool(Item*)> condition)
{
	for (auto & item : _Items)
	{
		if (condition(item.second))
			return item.second;
	}

	return nullptr;
}
