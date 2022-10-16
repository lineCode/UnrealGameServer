#include "pch.h"
#include "ObjectUtils.h"
#include "GameObject.h"

/*---------------------------------------------------------------------------------------------
이름     : ObjectUtils::GetObjectType
용도     : ID에 저장되어 있는 ObjectType을 반환
수정자   : 이민규
수정날짜 : 2022.09.25
----------------------------------------------------------------------------------------------*/
Protocol::ObjectType ObjectUtils::GetObjectType(int32 id)
{
	id = id & 0x7F000000;
	int32 type = (id >> 24) & 0x7FFF;
	return static_cast<Protocol::ObjectType>(type);
}

/*---------------------------------------------------------------------------------------------
이름     : ObjectUtils::SetStatByObject
용도     : object의 stat로 받은 stat 값을 세팅하는 함수
수정자   : 이민규
수정날짜 : 2022.10.03
----------------------------------------------------------------------------------------------*/
void ObjectUtils::SetStatByObject(Protocol::StatInfo * stat, GameObject* object)
{
	stat->set_hp(object->GetStat().hp());
	stat->set_maxhp(object->GetStat().maxhp());
	stat->set_speed(object->GetStat().speed());
}

/*---------------------------------------------------------------------------------------------
이름     : ObjectUtils::SetVectorByObejct
용도     : object의 Vector로 받은 Vector 값을 세팅하는 함수
수정자   : 이민규
수정날짜 : 2022.09.25
----------------------------------------------------------------------------------------------*/
void ObjectUtils::SetVectorByObejct(Protocol::Vector* vector, GameObject* object)
{
	vector->set_x(object->GetVector().x());
	vector->set_y(object->GetVector().y());
	vector->set_z(object->GetVector().z());
}

/*---------------------------------------------------------------------------------------------
이름     : ObjectUtils::SetVectorByVector
용도     : Des Vector에 받은 SRC Vector로 값을 세팅하는 함수
수정자   : 이민규
수정날짜 : 2022.09.25
----------------------------------------------------------------------------------------------*/
void ObjectUtils::SetVectorByVector(Protocol::Vector* desvector, const Protocol::Vector& srcvector)
{
	desvector->set_x(srcvector.x());
	desvector->set_y(srcvector.y());
	desvector->set_z(srcvector.z());
}

/*---------------------------------------------------------------------------------------------
이름     : ObjectUtils::SetVectorByObejct
용도     : object의 Rector로 받은 Rector 값을 세팅하는 함수
수정자   : 이민규
수정날짜 : 2022.09.25
----------------------------------------------------------------------------------------------*/
void ObjectUtils::SetRotatorByObject(Protocol::Rotator* rotator, GameObject* object)
{
	rotator->set_pitch(object->GetRotator().pitch());
	rotator->set_yaw(object->GetRotator().yaw());
	rotator->set_roll(object->GetRotator().roll());
}

/*---------------------------------------------------------------------------------------------
이름     : ObjectUtils::SetRotatorByRotator
용도     : Des Rotator에 받은 SRC Rotator로 값을 세팅하는 함수
수정자   : 이민규
수정날짜 : 2022.09.25
----------------------------------------------------------------------------------------------*/
void ObjectUtils::SetRotatorByRotator(Protocol::Rotator* desrotator, const Protocol::Rotator& srcrotator)
{
	desrotator->set_pitch(srcrotator.pitch());
	desrotator->set_yaw(srcrotator.yaw());
	desrotator->set_roll(srcrotator.roll());
}

/*---------------------------------------------------------------------------------------------
이름     : ObjectUtils::SetStat
용도     : src 에 받은 값으로 des 스텟을 세팅하는 함수
수정자   : 이민규
수정날짜 : 2022.10.17
----------------------------------------------------------------------------------------------*/
void ObjectUtils::SetStat(Protocol::StatInfo * src, Protocol::StatInfo * des)
{
	des->set_level(src->level());
	des->set_maxhp(src->maxhp());
	des->set_hp(src->hp());
	des->set_hp(src->hp());
	des->set_speed(src->speed());
	des->set_totalexp(src->totalexp());
}

/*---------------------------------------------------------------------------------------------
이름     : ObjectUtils::SetEnterPacket
용도     : EnterPacket을 Object의 값으로 세팅해주는 함수
수정자   : 이민규
수정날짜 : 2022.10.03
----------------------------------------------------------------------------------------------*/
void ObjectUtils::SetEnterPacket(Protocol::SERVER_ENTERGAME& enterpacket, GameObject* object)
{
	enterpacket.mutable_player()->set_objectid(object->GetInfo().objectid());
	enterpacket.mutable_player()->set_name(object->GetInfo().name());

	auto stat = enterpacket.mutable_player()->mutable_statinfo();
	SetStatByObject(stat, object);

	auto vector = enterpacket.mutable_player()->mutable_vector();
	SetVectorByObejct(vector, object);

	auto rotator = enterpacket.mutable_player()->mutable_rotator();
	SetRotatorByObject(rotator, object);
}

/*---------------------------------------------------------------------------------------------
이름     : ObjectUtils::SetSpawnPacket
용도     : SpawnPacket을 Object의 값으로 세팅해주는 함수
수정자   : 이민규
수정날짜 : 2022.10.03
----------------------------------------------------------------------------------------------*/
Protocol::ObjectInfo* ObjectUtils::SetSpawnPacket(Protocol::SERVER_SPAWN& spawnpacket, GameObject* object)
{
	Protocol::ObjectInfo* objeectinfo = spawnpacket.add_objects();
	objeectinfo->set_objectid(object->GetInfo().objectid());
	objeectinfo->set_name(object->GetInfo().name());

	auto stat = objeectinfo->mutable_statinfo();
	SetStatByObject(stat, object);

	auto vector = objeectinfo->mutable_vector();
	SetVectorByObejct(vector, object);

	auto rotator = objeectinfo->mutable_rotator();
	SetRotatorByObject(rotator, object);

	return objeectinfo;
}

/*---------------------------------------------------------------------------------------------
이름     : ObjectUtils::SetMovePacket
용도     : MovePacket을 Object의 값으로 세팅해주는 함수
수정자   : 이민규
수정날짜 : 2022.09.28
----------------------------------------------------------------------------------------------*/
void ObjectUtils::SetMovePacket(Protocol::SERVER_MOVE& movepacket, GameObject* object)
{
	movepacket.set_objectid(object->GetId());

	auto vector = movepacket.mutable_vector();

	SetVectorByObejct(vector, object);
}

/*---------------------------------------------------------------------------------------------
이름     : ObjectUtils::SetSkillPacket
용도     : SkillPacket을 Object와 skillpkt 값으로 세팅해주는 함수
수정자   : 이민규
수정날짜 : 2022.09.25
----------------------------------------------------------------------------------------------*/
void ObjectUtils::SetSkillPacket(Protocol::SERVER_SKILL& skillpacket, GameObject* object, Protocol::CLIENT_SKILL& objectpkt)
{
	skillpacket.set_objectid(object->GetInfo().objectid());

	auto skillinfo = skillpacket.mutable_skillinfo();
	skillinfo->set_skillid(objectpkt.skillinfo().skillid());

	auto vector = skillinfo->mutable_skillvector();
	auto rotaor = skillinfo->mutable_skillrotator();

	SetVectorByVector(vector, objectpkt.skillinfo().skillvector());
	SetRotatorByRotator(rotaor, objectpkt.skillinfo().skillrotator());
}



