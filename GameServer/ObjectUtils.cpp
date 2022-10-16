#include "pch.h"
#include "ObjectUtils.h"
#include "GameObject.h"

/*---------------------------------------------------------------------------------------------
�̸�     : ObjectUtils::GetObjectType
�뵵     : ID�� ����Ǿ� �ִ� ObjectType�� ��ȯ
������   : �̹α�
������¥ : 2022.09.25
----------------------------------------------------------------------------------------------*/
Protocol::ObjectType ObjectUtils::GetObjectType(int32 id)
{
	id = id & 0x7F000000;
	int32 type = (id >> 24) & 0x7FFF;
	return static_cast<Protocol::ObjectType>(type);
}

/*---------------------------------------------------------------------------------------------
�̸�     : ObjectUtils::SetStatByObject
�뵵     : object�� stat�� ���� stat ���� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.03
----------------------------------------------------------------------------------------------*/
void ObjectUtils::SetStatByObject(Protocol::StatInfo * stat, GameObject* object)
{
	stat->set_hp(object->GetStat().hp());
	stat->set_maxhp(object->GetStat().maxhp());
	stat->set_speed(object->GetStat().speed());
}

/*---------------------------------------------------------------------------------------------
�̸�     : ObjectUtils::SetVectorByObejct
�뵵     : object�� Vector�� ���� Vector ���� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.09.25
----------------------------------------------------------------------------------------------*/
void ObjectUtils::SetVectorByObejct(Protocol::Vector* vector, GameObject* object)
{
	vector->set_x(object->GetVector().x());
	vector->set_y(object->GetVector().y());
	vector->set_z(object->GetVector().z());
}

/*---------------------------------------------------------------------------------------------
�̸�     : ObjectUtils::SetVectorByVector
�뵵     : Des Vector�� ���� SRC Vector�� ���� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.09.25
----------------------------------------------------------------------------------------------*/
void ObjectUtils::SetVectorByVector(Protocol::Vector* desvector, const Protocol::Vector& srcvector)
{
	desvector->set_x(srcvector.x());
	desvector->set_y(srcvector.y());
	desvector->set_z(srcvector.z());
}

/*---------------------------------------------------------------------------------------------
�̸�     : ObjectUtils::SetVectorByObejct
�뵵     : object�� Rector�� ���� Rector ���� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.09.25
----------------------------------------------------------------------------------------------*/
void ObjectUtils::SetRotatorByObject(Protocol::Rotator* rotator, GameObject* object)
{
	rotator->set_pitch(object->GetRotator().pitch());
	rotator->set_yaw(object->GetRotator().yaw());
	rotator->set_roll(object->GetRotator().roll());
}

/*---------------------------------------------------------------------------------------------
�̸�     : ObjectUtils::SetRotatorByRotator
�뵵     : Des Rotator�� ���� SRC Rotator�� ���� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.09.25
----------------------------------------------------------------------------------------------*/
void ObjectUtils::SetRotatorByRotator(Protocol::Rotator* desrotator, const Protocol::Rotator& srcrotator)
{
	desrotator->set_pitch(srcrotator.pitch());
	desrotator->set_yaw(srcrotator.yaw());
	desrotator->set_roll(srcrotator.roll());
}

/*---------------------------------------------------------------------------------------------
�̸�     : ObjectUtils::SetStat
�뵵     : src �� ���� ������ des ������ �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.17
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
�̸�     : ObjectUtils::SetEnterPacket
�뵵     : EnterPacket�� Object�� ������ �������ִ� �Լ�
������   : �̹α�
������¥ : 2022.10.03
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
�̸�     : ObjectUtils::SetSpawnPacket
�뵵     : SpawnPacket�� Object�� ������ �������ִ� �Լ�
������   : �̹α�
������¥ : 2022.10.03
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
�̸�     : ObjectUtils::SetMovePacket
�뵵     : MovePacket�� Object�� ������ �������ִ� �Լ�
������   : �̹α�
������¥ : 2022.09.28
----------------------------------------------------------------------------------------------*/
void ObjectUtils::SetMovePacket(Protocol::SERVER_MOVE& movepacket, GameObject* object)
{
	movepacket.set_objectid(object->GetId());

	auto vector = movepacket.mutable_vector();

	SetVectorByObejct(vector, object);
}

/*---------------------------------------------------------------------------------------------
�̸�     : ObjectUtils::SetSkillPacket
�뵵     : SkillPacket�� Object�� skillpkt ������ �������ִ� �Լ�
������   : �̹α�
������¥ : 2022.09.25
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



