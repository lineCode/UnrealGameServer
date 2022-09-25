#pragma once
#include "Protocol.pb.h"

class GameObject;

/*----------------------------------------------------------------------------------------------------
이름     : ObjectUtils
용도     : Object를 세팅 및 관련 항목을 생성 해주는 유틸 객체
수정자   : 이민규
수정날짜 : 2022.09.25
-----------------------------------------------------------------------------------------------------*/
class ObjectUtils
{
public:
	static Protocol::ObjectType GetObjectType(int32 id);

	static void SetVectorByObejct(Protocol::Vector* vector, GameObject* object);
	static void SetVectorByVector(Protocol::Vector* desvector, const  Protocol::Vector& srcvector);
	static void SetRotatorByObject(Protocol::Rotator* rotator, GameObject* object);
	static void SetRotatorByRotator(Protocol::Rotator* desrotator, const Protocol::Rotator& srcrotator);

	static void SetEnterPacket(Protocol::SERVER_ENTERGAME& enterpacket, GameObject* object);
	static Protocol::ObjectInfo* SetSpawnPacket(Protocol::SERVER_SPAWN& spawnpacket, GameObject* object);
	static void SetMovePacket(Protocol::SERVER_MOVE& movepacket, GameObject* object);
	static void SetSkillPacket(Protocol::SERVER_SKILL& skillpacket, GameObject* object, Protocol::CLIENT_SKILL& objectpkt);
};

