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

	static void SetStatByObject(Protocol::StatInfo* stat, GameObject* object);
	static void SetVectorByObejct(Protocol::Vector* vector, GameObject* object);
	static void SetVectorByVector(Protocol::Vector* desvector, const  Protocol::Vector& srcvector);
	static void SetRotatorByObject(Protocol::Rotator* rotator, GameObject* object);
	static void SetRotatorByRotator(Protocol::Rotator* desrotator, const Protocol::Rotator& srcrotator);

	static void SetEnterPacket(Protocol::SERVER_ENTERGAME& enterpacket, GameObject* object);
	static Protocol::ObjectInfo* SetSpawnPacket(Protocol::SERVER_SPAWN& spawnpacket, GameObject* object);
	static void SetMovePacket(Protocol::SERVER_MOVE& movepacket, GameObject* object);
	static void SetSkillPacket(Protocol::SERVER_SKILL& skillpacket, GameObject* object, Protocol::CLIENT_SKILL& objectpkt);

	static int32 sqrMagnitude(Protocol::Vector vector) { return (vector.x() * vector.x()) + (vector.y() * vector.y()) + (vector.z() * vector.z()); }
	static float Magnitude(Protocol::Vector vector) { return static_cast<float>(sqrt(sqrMagnitude(vector))); }
	static int VectorDistanceZero(Protocol::Vector  vector) {return abs( vector.x()) + abs(vector.y()) + abs(vector.z());}
};

/*----------------------------------------------------------------------------------------------------
�̸�     : Vector ������ �������̵�
�뵵     : Vector�� �پ��� ����� �������̵��� �����ϱ� ���� ���۷����͵�
������   : �̹α�
������¥ : 2022.09.30
-----------------------------------------------------------------------------------------------------*/
static Protocol::Vector operator+(Protocol::Vector  vec1, Protocol::Vector  vc2)
{
	Protocol::Vector result;
	result.set_x(vec1.x() + vc2.x());
	result.set_y(vec1.y() + vc2.y());
	result.set_z(vec1.z() + vc2.z());

	return result;
}

static Protocol::Vector operator-(Protocol::Vector vec1, Protocol::Vector vc2)
{
	Protocol::Vector result;
	result.set_x(vec1.x() - vc2.x());
	result.set_y(vec1.y() - vc2.y());
	result.set_z(vec1.z() - vc2.z());

	return result;
}