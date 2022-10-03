#include "pch.h"
#include "GameObject.h"
#include <format>
#include "GameRoom.h"
#include "ServerPacketManager.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
	SetRoom(nullptr);
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameObject::OnDamaged
�뵵     : �������� ������ �Լ�
           attacker�� ���� ���� ����� �������� �� �� ����
������   : �̹α�
������¥ : 2022.10.03
----------------------------------------------------------------------------------------------*/
void GameObject::OnDamaged(GameObject* attacker, int damage)
{
	if (_Room == nullptr)
		return;

	SetHp(GetHp() - damage);

	if(GetHp() <= 0)
		OnDead(attacker);

	Protocol::SERVER_CHANGEHP pkthp;
	pkthp.set_objectid(_Info.objectid());
	pkthp.set_hp(GetHp());

	_Room->BroadCast(ServerPacketManager::MakeSendBuffer(pkthp));
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameObject::OnDead
�뵵     : ������Ʈ�� �׾��� �� ȣ�� �Ǵ� �Լ�
������   : �̹α�
������¥ : 2022.10.03
----------------------------------------------------------------------------------------------*/
void GameObject::OnDead(GameObject* attacker)
{
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameObject::SetStat
�뵵     : ���� Stat ���� newstat������ �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.03
----------------------------------------------------------------------------------------------*/
void GameObject::SetStat(Protocol::StatInfo newstat)
{
	auto stat = _Info.mutable_statinfo();
	stat->set_level(newstat.level());
	stat->set_maxhp(newstat.hp());
	stat->set_hp(newstat.hp());
	stat->set_speed(newstat.speed());
	stat->set_totalexp(newstat.totalexp());
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameObject::SetVector
�뵵     : ���� Vector���� newvector������ �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.09.19
----------------------------------------------------------------------------------------------*/
void GameObject::SetVector(Protocol::Vector newvector)
{
	auto vector = _Info.mutable_vector();
	vector->set_x(newvector.x());
	vector->set_y(newvector.y());
	vector->set_z(newvector.z());
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameObject::SetRotator
�뵵     : ���� Rotator���� newroator������ �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.09.19
----------------------------------------------------------------------------------------------*/
void GameObject::SetRotator(Protocol::Rotator newrotator)
{
	auto rotator = _Info.mutable_rotator();
	rotator->set_pitch(newrotator.pitch());
	rotator->set_yaw(newrotator.yaw());
	rotator->set_roll(newrotator.roll());
}

void GameObject::Update()
{
}

