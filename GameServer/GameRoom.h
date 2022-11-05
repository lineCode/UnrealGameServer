#pragma once
#include "Protocol.pb.h"

class GameObject;
class Player;
class Monster;
class Projectile;
class Zone;

struct Size
{
	int32 x;
	int32 y;
};

/*---------------------------------------------------------------------------------------------
이름     : GameRoom
용도     : 플레이어들이 입장할 수 있는 공간을 만드는 객체
수정자   : 이민규
수정날짜 : 2022.10.5
----------------------------------------------------------------------------------------------*/
class GameRoom : public JobQueue
{

#pragma region GameRoom
public:
	GameRoom();
	shared_ptr<GameRoom> GetGameRoom() { return  static_pointer_cast<GameRoom>(shared_from_this()); }

	GetSetMaker(int32, RoomId, _RoomId)

	void MonsterUpdate(Monster * monster);
	void VisionUpdate(Player * player);

	Player* FindPlayer(function<bool(Player*)> condition);

	void Init(int32 mapid , int32 zonesize = 2000);
	void EnterGame(GameObject* gameobject);
	void LeaveGame(int32 objectid);

	void OnDamage(Protocol::CLIENT_DAMAGE pkt);
	void BroadCast(shared_ptr<class SendBuffer> sendbuffer , Protocol::Vector pos);

	Zone* GetZone(Protocol::Vector pos);
	GhashSet<Zone*> GetAdjacentZones(Protocol::Vector pos, int32 cell = 1000);

public:
	// 플레이어 시야각
	int32 VisionCells = 500;

private:
	int32 _RoomId = 0;
	int32 _ZoneSize = 0;
	Size _MapSize = { 0 , 0 };

	Gvector<Gvector<Zone*>> _Zones;

	GhashMap<int32, Player*> _Players;
	GhashMap<int32, Monster*> _Monsters;
	GhashMap<int32, Projectile*> _Projectiles;
#pragma endregion

#pragma region Battle
public:
	void PlayerMove(Player* player, Protocol::CLIENT_MOVE * pkt);
	void PlayerSkill(Player* player, Protocol::CLIENT_SKILL* pkt);
#pragma endregion

#pragma region Item
	void PlayerEquipItem(Player* player, Protocol::CLIENT_EQUIPITEM *pkt);
#pragma endregion
};

