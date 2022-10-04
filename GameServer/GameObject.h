#pragma once

#include "Protocol.pb.h"

class GameRoom;

/*---------------------------------------------------------------------------------------------
이름     : GameObject
용도     : 서버에 Object의 최상위 클래스
수정자   : 이민규
수정날짜 : 2022.09.24
----------------------------------------------------------------------------------------------*/
class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	Protocol::ObjectType GetObjectType() { return _ObjectType; }
	void SetObjectType(Protocol::ObjectType type) { _ObjectType = type; }

	int32 GetType() { return _Type; }
	void SetType(int32 type) { _Type = type; }

	GameRoom* GetRoom() { return _Room; }
	void SetRoom(GameRoom* room) { _Room = room; }

	Protocol::ObjectInfo& GetInfo() { return _Info; }
	void SetInfo(Protocol::ObjectInfo info) { _Info = info; }

	Protocol::StatInfo GetStat() { return _Info.statinfo(); }
	void SetStat(Protocol::StatInfo  newstat);

	Protocol::Vector GetVector() { return _Info.vector(); }
	void SetVector(Protocol::Vector newvector);

	Protocol::Rotator GetRotator() { return _Info.rotator(); }
	void SetRotator(Protocol::Rotator newrotator);

	int32 GetId() { return _Info.objectid(); }
	void SetId(int32 id) { return _Info.set_objectid(id); }

	int32 GetHp() { return _Info.statinfo().hp(); }
	void SetHp(int32 hp) { _Info.mutable_statinfo()->set_hp( std::clamp(hp, 0, GetStat().maxhp())); }

	float GetSpeed() { return _Info.statinfo().speed(); }
	void SetSpeed(float speed) { return _Info.mutable_statinfo()->set_speed(speed); }


	virtual void Update();
	virtual void OnDamaged(GameObject * attacker , int damage);
	virtual void OnDead(GameObject * attacker);

protected:
	Protocol::ObjectType _ObjectType = Protocol::ObjectType::OBJECT_NONE;

	Protocol::ObjectState _State = Protocol::ObjectState::IDLE;

	Protocol::ObjectInfo _Info;

	GameRoom* _Room = nullptr;

	int32 _Type = 0;
};

