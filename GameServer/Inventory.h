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
	~Inventory();
	void Add(Item* item);
	Item* GetItem(int itemid);
	Item* Find(function<bool(Item*)> condition);

private:
	GhashMap<int32, Item*> _Items;
};

