#pragma once
#include "Protocol.pb.h"

class GameObject;

/*----------------------------------------------------------------------------------------------------
�̸�     : ObjectUtils
�뵵     : Object�� ���� �� ���� �׸��� ���� ���ִ� ��ƿ ��ü
������   : �̹α�
������¥ : 2022.09.25
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

