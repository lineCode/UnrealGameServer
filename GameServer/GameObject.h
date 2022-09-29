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
	virtual ~GameObject();

	Protocol::ObjectType GetObjectType() { return _ObjectType; }
	void SetObjectType(Protocol::ObjectType type) { _ObjectType = type; }

	int32 GetType() { return _Type; }
	void SetType(int32 type) { _Type = type; }

	GameRoom* GetRoom() { return _Room; }
	void SetRoom(GameRoom* room) { _Room = room; }

	Protocol::ObjectInfo& GetInfo() { return _Info; }
	void SetInfo(Protocol::ObjectInfo info) { _Info = info; }

	Protocol::Vector GetVector() { return _Info.vector(); }
	void SetVector(Protocol::Vector newvector);

	Protocol::Rotator GetRotator() { return _Info.rotator(); }
	void SetRotator(Protocol::Rotator newrotator);

	int32 GetId() { return _Info.objectid(); }
	void SetId(int32 id) { return _Info.set_objectid(id); }

	virtual void Update();

protected:
	Protocol::ObjectType _ObjectType = Protocol::ObjectType::OBJECT_NONE;

	int32 _Type = 0;

	Protocol::ObjectState _ObjectState = Protocol::ObjectState::IDLE;

	Protocol::ObjectInfo _Info;
	GameRoom* _Room = nullptr;

	
};

