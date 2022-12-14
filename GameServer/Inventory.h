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
	Inventory();
	~Inventory();
	void Add(Item* item);
	void Erase(Item* item);
	Item* Find(int32 itemdbid);
	Item* FindCondition(function<bool(Item*)> condition);
	int32 GetEmptySlot();

	GhashMap<int32, Item*> GetItems() const { return _Items; }
private:
	GhashMap<int32, Item*> _Items;
	bool slots[20];
};

