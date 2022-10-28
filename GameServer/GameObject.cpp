#include "pch.h"
#include "GameObject.h"
#include <random>
#include "GameRoom.h"
#include "Monster.h"
#include "ObjectManager.h"
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

	damage =  max(damage - TotalDefence(), 0);
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
	if (_Room == nullptr)
		return;

	Protocol::SERVER_DIE pktdie;
	pktdie.set_objectid(GetId());
	pktdie.set_attackerid(attacker->GetId());
	_Room->BroadCast(ServerPacketManager::MakeSendBuffer(pktdie));

	auto room = _Room;

	room->PushAsync(&GameRoom::LeaveGame, GetId());

	// TODO : 추후 몬스터 리스폰 전부 변경

	std::random_device rd;

	std::mt19937_64 gen(rd());

	std::uniform_int_distribution dis(-1000, 0);

	Monster* monster = ObjectManager::GetInstance().Add<Monster>(Protocol::MonsterType::SAVAROG);
	monster->Init(1);

	Protocol::Vector vector;
	vector.set_x(dis(rd));
	vector.set_y(dis(rd));
	vector.set_z(97.6f);
	monster->SetVector(vector);

	room->PushAsync(&GameRoom::EnterGame, static_cast<GameObject*>(monster));
}

void GameObject::Update()
{
}

