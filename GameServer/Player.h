#pragma once
#include "GameObject.h"
#include "Protocol.pb.h"

class GameRoom;
class ServerSession;
class Inventory;
class Equipment;

/*---------------------------------------------------------------------------------------------
�̸�     : Player
�뵵     : �������� �÷��̾� ������ ������ ���� ��ü
������   : �̹α�
������¥ : 2022.09.19
----------------------------------------------------------------------------------------------*/
class Player :public  GameObject
{
public:
	Player();
	virtual ~Player() override;

	GetSetMaker(shared_ptr<ServerSession>, Session, _Session)
	GetSetMaker(int32, PlayerDbId, _playerdbid)
	GetSetMaker(Inventory*, Inventory, _Inventory)
	GetSetMaker(Equipment*, Equipment, _Equipment)
	GetSetMaker(int32, WeaponAttack, _WeaponAttack)
	GetSetMaker(int32, ArmorDefence, _ArmorDefence)

	void LeaveGame();
	void ItemEquip(Protocol::CLIENT_EQUIPITEM* pkt);
	void RefreshStat();

	virtual int32 TotalAttack() override { return GetStat().attack() + _WeaponAttack; } 
	virtual int32 TotalDefence() override { return _ArmorDefence; }

private:
	shared_ptr<ServerSession> _Session = nullptr;
	int32 _playerdbid = 0;
	Inventory* _Inventory = nullptr;
	Equipment* _Equipment = nullptr;

	int32 _WeaponAttack = 0;
	int32 _ArmorDefence = 0;
};
