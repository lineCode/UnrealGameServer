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

	GetSetMaker(Protocol::ObjectType, ObjectType, _ObjectType)

	GetSetMaker(int32, Type, _Type)

	GetSetMaker(shared_ptr<GameRoom>, Room, _Room)

	ProtoGetSetMaker(Protocol::StatInfo, Stat, _Info.statinfo, _Info.mutable_statinfo()->MergeFrom)

	ProtoGetSetMaker(Protocol::Vector, Vector, _Info.vector, _Info.mutable_vector()->MergeFrom)

	ProtoGetSetMaker(Protocol::Rotator, Rotator, _Info.rotator, _Info.mutable_rotator()->MergeFrom)

	ProtoGetSetMaker(int32, Id, _Info.objectid , _Info.set_objectid)

	ProtoGetSetMaker(int32, Speed, _Info.statinfo().speed, _Info.mutable_statinfo()->set_speed)

	Protocol::ObjectInfo& GetInfo() { return _Info; }
	void SetInfo(Protocol::ObjectInfo info) { _Info = info; }

	int32 GetHp() { return _Info.statinfo().hp(); }
	void SetHp(int32 hp) { _Info.mutable_statinfo()->set_hp( std::clamp(hp, 0, GetStat().maxhp())); }

	virtual void Update();
	virtual void OnDamaged(GameObject * attacker , int damage);
	virtual void OnDead(GameObject * attacker);

	virtual int32 TotalAttack() { return GetStat().attack(); }
	virtual int32 TotalDefence() { return 0; }

	virtual GameObject* GetOwner() { return this; }

protected:
	Protocol::ObjectType _ObjectType = Protocol::ObjectType::OBJECT_NONE;

	Protocol::ObjectState _State = Protocol::ObjectState::IDLE;

	Protocol::ObjectInfo _Info;

	shared_ptr<GameRoom> _Room = nullptr;

	int32 _Type = 0;
};

