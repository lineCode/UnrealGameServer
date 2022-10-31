#pragma once
#include "Protocol.pb.h"

class GameObject;
class Player;
class Monster;
class Projectile;

/*---------------------------------------------------------------------------------------------
�̸�     : GameRoom
�뵵     : �÷��̾���� ������ �� �ִ� ������ ����� ��ü
������   : �̹α�
������¥ : 2022.10.5
----------------------------------------------------------------------------------------------*/
class GameRoom : public JobQueue
{

#pragma region GameRoom
public:
	GameRoom();
	shared_ptr<GameRoom> GetGameRoom() { return  static_pointer_cast<GameRoom>(shared_from_this()); }

	void SetRoomId(int32 id) { _RoomId = id; }
	int32 GetRoomId() { return _RoomId; }
	void MonsterUpdate(Monster * monster);

	Player* FindPlayer(function<bool(Player*)> condition);

	void Init(int32 mapid);
	void EnterGame(GameObject* gameobject);
	void LeaveGame(int32 objectid);

	void OnDamage(Protocol::CLIENT_DAMAGE pkt);
	void BroadCast(shared_ptr<class SendBuffer> sendbuffer);

private:
	int32 _RoomId = 0;
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

