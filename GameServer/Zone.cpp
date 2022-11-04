#include "pch.h"
#include "Zone.h"

Zone::Zone(int32 y, int32 x) : _PosY(y) , _PosX(x)
{
}

/*---------------------------------------------------------------------------------------------
�̸�     : Zone::FindPlayerCondtion
�뵵     : ���ǿ� �´� �÷��̾� �� ���� ù��°�� ��ȯ�ϴ� �Լ�
           �� ���ǿ� �´� �÷��̾ ���� ��� nullptr
������   : �̹α�
������¥ : 2022.11.4
----------------------------------------------------------------------------------------------*/
Player* Zone::FindPlayerCondtion(function<Player*(bool)> condition)
{
	for (auto player : _Players)
	{
		if (condition(player))
			return player;
	}

	return nullptr;
}

/*---------------------------------------------------------------------------------------------
�̸�     : Zone::FindPlayerAillCondtion
�뵵     : ���ǿ� �´� �÷��̾� ��θ� Gvector�� �Ѱ��ִ� �Լ�
		   �� ���ǿ� �´� �÷��̾ ���� ��� list size = 0
������   : �̹α�
������¥ : 2022.11.4
----------------------------------------------------------------------------------------------*/
Gvector<Player*> Zone::FindPlayerAillCondtion(function<Player*(bool)> condition)
{
	Gvector<Player*> findlist;

	for (auto player : _Players)
	{
		if (condition(player))
			findlist.push_back(player);
	}

	return findlist;
}
