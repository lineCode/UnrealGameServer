#pragma once
#include "Protocol.pb.h"

class GameObject;
class Player;
class Monster;
class Projectile;

/*---------------------------------------------------------------------------------------------
이름     : GameRoom
용도     : 플레이어들이 입장할 수 있는 공간을 만드는 객체
수정자   : 이민규
수정날짜 : 2022.09.12
----------------------------------------------------------------------------------------------*/
class GameRoom
{
public:
	GameRoom();
	void Init(int32 mapid);
	void EnterGame(GameObject* gameobject);
	void LeaveGame(int32 objectid);

	void SetRoomId(int32 id) { _RoomId = id; }
	int32 GetRoomId() { return _RoomId; }

	void PlayerMove(Player* player, Protocol::CLIENT_MOVE& pkt);
	void PlayerSkill(Player* player, Protocol::CLIENT_SKILL& pkt);
	void BroadCast(shared_ptr<class SendBuffer> sendbuffer);

	void update();

	Player* FindPlayer(function<bool(Player*)> condition);
private:
	RWLOCK_USE;

	int32 _RoomId = 0;
	GhashMap<int32 , Player*> _Players;
	GhashMap<int32 , Monster*> _Monsters;
	GhashMap<int32 , Projectile*> _Projectiles;
};

