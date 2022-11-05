#pragma once

/*---------------------------------------------------------------------------------------------
�̸�     : Zone
�뵵     : �� �ʿ��� ������ ������ ���� ��ü
������   : �̹α�
������¥ : 2022.11.4
----------------------------------------------------------------------------------------------*/

class Player;
class Monster;

class Zone
{
public:
	Zone(int32 y, int32 x);

	GetSetMaker(int32, PosX, _PosX);
	GetSetMaker(int32, PosY, _PosY);
	GetSetMaker(GhashSet<Player*>, Players, _Players);
	GetSetMaker(GhashSet<Monster*>, Monsters, _Monsters);

	void AddPlayer(Player* player) { _Players.insert(player); }
	void RemovePlayer(Player* player) { _Players.erase(player); }

	void AddMonster(Monster* monster) { _Monsters.insert(monster); }
	void RemoveMonster(Monster* monster) { _Monsters.erase(monster); }

	Player* FindPlayerCondtion(function<Player*(bool)> condition);
	Gvector<Player*> FindPlayerAillCondtion(function<Player* (bool)> condition);

private:
	int32 _PosX = 0;
	int32 _PosY = 0;

	GhashSet<Player*> _Players;
	GhashSet<Monster*> _Monsters;
};

