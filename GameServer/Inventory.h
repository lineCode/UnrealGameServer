#pragma once

class Item;

/*---------------------------------------------------------------------------------------------
�̸�     : Inventory
�뵵     : �÷��̾��� �������� �����ϴ� ��ü
������   : �̹α�
������¥ : 2022.10.19
----------------------------------------------------------------------------------------------*/
class Inventory
{
public:
	Inventory();
	~Inventory();
	void Add(Item* item);
	Item* GetItem(int itemid);
	Item* Find(function<bool(Item*)> condition);
	int32 GetEmptySlot();

private:
	GhashMap<int32, Item*> _Items;
	bool slots[20];
};

