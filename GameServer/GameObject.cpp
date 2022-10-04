#include "pch.h"
#include "GameObject.h"

#include <format>
#include <random>

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
이름     : GameObject::OnDamaged
용도     : 데미지를 입히는 함수
           attacker를 통해 공격 대상이 누구인지 알 수 있음
수정자   : 이민규
수정날짜 : 2022.10.03
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
이름     : GameObject::OnDead
용도     : 오브젝트가 죽었을 때 호출 되는 함수
수정자   : 이민규
수정날짜 : 2022.10.04
----------------------------------------------------------------------------------------------*/
void GameObject::OnDead(GameObject* attacker)
{
	Protocol::SERVER_DIE pktdie;
	pktdie.set_objectid(GetId());
	pktdie.set_attackerid(attacker->GetId());
	_Room->BroadCast(ServerPacketManager::MakeSendBuffer(pktdie));

	GameRoom* room = _Room;

	_Room->LeaveGame(GetId());

	std::random_device rd;

	std::mt19937_64 gen(rd());

	std::uniform_int_distribution dis(-1000, 0);

	SetHp(GetStat().maxhp());
	Protocol::Vector vector;
	vector.set_x(dis(gen));
	vector.set_y(dis(gen));
	vector.set_z(97.f);
	SetVector(vector);

	room->EnterGame(this);
}

/*---------------------------------------------------------------------------------------------
이름     : GameObject::SetStat
용도     : 나의 Stat 값을 newstat값으로 수정하는 함수
수정자   : 이민규
수정날짜 : 2022.10.03
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
이름     : GameObject::SetVector
용도     : 나의 Vector값을 newvector값으로 수정하는 함수
수정자   : 이민규
수정날짜 : 2022.09.19
----------------------------------------------------------------------------------------------*/
void GameObject::SetVector(Protocol::Vector newvector)
{
	auto vector = _Info.mutable_vector();
	vector->set_x(newvector.x());
	vector->set_y(newvector.y());
	vector->set_z(newvector.z());
}

/*---------------------------------------------------------------------------------------------
이름     : GameObject::SetRotator
용도     : 나의 Rotator값을 newroator값으로 수정하는 함수
수정자   : 이민규
수정날짜 : 2022.09.19
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

