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
이름     : Vector 연산자 오버라이딩
용도     : Vector를 다양한 방식의 오버라이딩을 지원하기 위한 오퍼레이터들
수정자   : 이민규
수정날짜 : 2022.09.30
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