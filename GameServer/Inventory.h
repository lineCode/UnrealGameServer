#pragma once

class Item;

/*---------------------------------------------------------------------------------------------
이름     : Inventory
용도     : 플레이어의 아이템을 보관하는 객체
수정자   : 이민규
수정날짜 : 2022.10.19
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

