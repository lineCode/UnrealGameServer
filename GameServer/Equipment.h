#pragma once

class Item;

/*---------------------------------------------------------------------------------------------
�̸�     : Equipment
�뵵     : �÷��̾ ������ �������� �����ϴ� ��ü
������   : �̹α�
������¥ : 2022.10.28
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

