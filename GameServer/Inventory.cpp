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
�̸�     : Inventory::Add
�뵵     : �κ��丮�� �������� �߰��ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.19
----------------------------------------------------------------------------------------------*/
void Inventory::Add(Item* item)
{
	slots[item->GetSlot()] = false;
	_Items.insert({ item->GetItemDbID() , item });
}

/*---------------------------------------------------------------------------------------------
�̸�     : Inventory::GetItem
�뵵     : �κ��丮�� �ִ� �������� �������� �Լ�
           ���� ��� nullptr ��ȯ
������   : �̹α�
������¥ : 2022.10.19
----------------------------------------------------------------------------------------------*/
Item* Inventory::GetItem(int itemid)
{
	auto item = _Items.find(itemid);

	if (item == _Items.end())
		return nullptr;
	
	return item->second;
}

/*---------------------------------------------------------------------------------------------
�̸�     : Inventory::Find
�뵵     : �κ��丮�� �ִ� �������� ���ǽĿ� ���� �������� �Լ�
           ���� ��� nullptr ��ȯ
������   : �̹α�
������¥ : 2022.10.19
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

/*---------------------------------------------------------------------------------------------
�̸�     : Inventory::GetEmptySlot
�뵵     : �κ��丮�� ������ �󽽷��� �˷��ִ� �Լ�
           Full �� ��� -1 ��ȯ
������   : �̹α�
������¥ : 2022.10.24
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
