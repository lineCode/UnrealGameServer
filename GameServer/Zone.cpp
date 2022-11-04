#include "pch.h"
#include "Zone.h"

Zone::Zone(int32 y, int32 x) : _PosY(y) , _PosX(x)
{
}

/*---------------------------------------------------------------------------------------------
이름     : Zone::FindPlayerCondtion
용도     : 조건에 맞는 플레이어 중 가장 첫번째를 반환하는 함수
           단 조건에 맞는 플레이어가 없을 경우 nullptr
수정자   : 이민규
수정날짜 : 2022.11.4
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
이름     : Zone::FindPlayerAillCondtion
용도     : 조건에 맞는 플레이어 모두를 Gvector로 넘겨주는 함수
		   단 조건에 맞는 플레이어가 없을 경우 list size = 0
수정자   : 이민규
수정날짜 : 2022.11.4
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
