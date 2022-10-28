#pragma once

class Item;

/*---------------------------------------------------------------------------------------------
이름     : Equipment
용도     : 플레이어가 장착한 아이템을 보관하는 객체
수정자   : 이민규
수정날짜 : 2022.10.28
----------------------------------------------------------------------------------------------*/
class Equipment
{
public:
	GhashMap<int32, Item*> GetItems() const { return _Items; }

	void Add(Item* item);
	void Erase(Item* item);
	Item* Find(int32 itemdbid);
	Item* FindCondition(function<bool(Item*)> condition);

private:
	GhashMap<int32, Item*> _Items;
};

