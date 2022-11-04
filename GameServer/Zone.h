#pragma once

/*---------------------------------------------------------------------------------------------
이름     : Zone
용도     : 한 맵에서 영역을 나누기 위한 객체
수정자   : 이민규
수정날짜 : 2022.11.4
----------------------------------------------------------------------------------------------*/

class Player;

class Zone
{
public:
	Zone(int32 y, int32 x);

	GetSetMaker(int32, PosX, _PosX);
	GetSetMaker(int32, PosY, _PosY);
	GetSetMaker(GhashSet<Player*>, Players, _Players);

	void AddPlayer(Player* player) { _Players.insert(player); }
	void RemovePlayer(Player* player) { _Players.erase(player); }

	Player* FindPlayerCondtion(function<Player*(bool)> condition);
	Gvector<Player*> FindPlayerAillCondtion(function<Player* (bool)> condition);

private:
	int32 _PosX = 0;
	int32 _PosY = 0;

	GhashSet<Player*> _Players;
};

