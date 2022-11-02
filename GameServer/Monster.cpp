#include "pch.h"
#include "Monster.h"
#include <format>
#include "DataManager.h"
#include "DBJobManager.h"
#include "Player.h"
#include "GameRoom.h"
#include "ObjectUtils.h"
#include "ServerPacketManager.h"

Monster::Monster()
{
	SetObjectType(Protocol::ObjectType::MONSTER);
}

Monster::~Monster()
{
	_jobs.clear();
}

void Monster::Init(int32 monsterid)
{
	_Monsterid = monsterid;

	MonsterData* monsterdata = DataManager::GetInstacnce()->GetMonsterData(monsterid);
	if (monsterdata == nullptr)
		GConsoleLogManager->WriteStdOut(Color::RED,L"[Monster] : %d Init Error", monsterid);

	SetStat(monsterdata->stat);

	_State = Protocol::ObjectState::IDLE;
}

/*---------------------------------------------------------------------------------------------
�̸�     : Monster::Update
�뵵     : ���� ���¿� ���� ������Ʈ ���ִ� �Լ�
           FSM (Finite State Machine) �ӽ� �⺻ ����
������   : �̹α�
������¥ : 2022.09.29
----------------------------------------------------------------------------------------------*/
void Monster::Update()
{
	switch (_State)
	{

	case Protocol::ObjectState::IDLE:
		{
			UpdateIdle();
			break;
		}

	case Protocol::ObjectState::MOVE:
		{
			UpdateMove();
			break;
		}

	case Protocol::ObjectState::SKILL:
		{
			UpdateSkill();
			break;
		}

	case Protocol::ObjectState::DEAD:
		{
			UpdateDead();
			break;
		}

	default: ;
	}

	// TODO : �̷��� ��� ������ vector�� ���̴� ������ ���� ���� ����
	// 5������ 0.2�ʸ��� �ѹ��� ������Ʈ
	if (_Room != nullptr)
		_jobs.push_back(_Room->TimerPush(200, &GameRoom::MonsterUpdate, this));
}

/*---------------------------------------------------------------------------------------------
�̸�     : Monster::UpdateIdle
�뵵     : ������ IDLE ���¸� ������Ʈ ���ִ� �Լ�
           �ڽŰ� ����� ĳ���͸� ã�� ã�� ��� Target���� �����ϰ� ���¸� MOVE�� ����
������   : �̹α�
������¥ : 2022.09.29
----------------------------------------------------------------------------------------------*/
void Monster::UpdateIdle()
{
	if (_NextSearchTick > ::GetTickCount64())
		return;

	_NextSearchTick = ::GetTickCount64() + 1000;

	Player * target = _Room->FindPlayer([&](Player * player)
		{
			Protocol::Vector dir = player->GetVector() - GetVector();
			if(ObjectUtils::VectorDistanceZero(dir) < _SearchDistance)
				return true;

			return false;
		});

	if (target == nullptr)
		return;

	_Target = target;

	_State = Protocol::ObjectState::MOVE;
}

/*---------------------------------------------------------------------------------------------
�̸�     : Monster::UpdateMove
�뵵     : ������ MOVE ���¸� ������Ʈ ���ִ� �Լ�
		   ���� ���� �Ÿ����� ChaseDistance ��ŭ �־����� ����ϰ�
		   �ƴϸ� Target���� �̵��ϴ� �Լ�
������   : �̹α�
������¥ : 2022.09.29
----------------------------------------------------------------------------------------------*/
void Monster::UpdateMove()
{
	if (_NextMoveTick > ::GetTickCount64())
		return;

	_NextMoveTick = ::GetTickCount64() + 1000;

	if(_Target == nullptr || _Target->GetRoom() != _Room)
	{
		_Target = nullptr;
		_State = Protocol::ObjectState::IDLE;
		BroadCastMove();
		return;
	}

	Protocol::Vector dir = _Target->GetVector() - GetVector();
	int32 dist = ObjectUtils::VectorDistanceZero(dir);

	if(dist == 0 || dist > _ChaseDistance)
	{
		_Target = nullptr;
		_State = Protocol::ObjectState::IDLE;
		BroadCastMove();
		return;
	}

	// TODO : ��ų ��� ���� üũ
	if(dist <= _SkillRange)
	{
		_CoolTimeTick = 0;
		_State = Protocol::ObjectState::SKILL;
		return;
	}

	// TODO : �̵�
	SetVector(_Target->GetVector());
}

void Monster::UpdateSkill()
{
	if(_CoolTimeTick == 0)
	{
		// TODO : ��ȿ�� Ÿ������
		if(_Target == nullptr || _Target->GetRoom() != _Room || _Target->GetHp() == 0)
		{
			_Target = nullptr;
			_State = Protocol::ObjectState::MOVE;
			BroadCastMove();
			return;
		}

		// TODO : ��ų�� ���� ��� ��������
		Protocol::Vector dir = _Target->GetVector() - GetVector();
		int32 dist = ObjectUtils::VectorDistanceZero(dir);
		if (dist <= _SkillRange)
		{
			_State = Protocol::ObjectState::MOVE;
			BroadCastMove();
			return;
		}

		// Ÿ���� ���� �ֽ�

		// ������ ����

		// ��ų ��� BroadCast

		// ��ų ��Ÿ�� ����
	}
}

void Monster::UpdateDead()
{
}

/*---------------------------------------------------------------------------------------------
�̸�     : Monster::OnDead
�뵵     : ���Ͱ� �׾��� �� ȣ�� �Ǵ� �Լ�
������   : �̹α�
������¥ : 2022.10.31
----------------------------------------------------------------------------------------------*/
void Monster::OnDead(GameObject* attacker)
{
	// ���Ͱ� ������ ���� ���� ������ ���� ��ҽ�Ŵ
	for(auto & job : _jobs)
		job->SetCancel(true);

	_jobs.clear();
	GameObject::OnDead(attacker);

	// ������ ����
	GameObject* owner = attacker->GetOwner();
	if(owner->GetObjectType() == Protocol::ObjectType::PLAYER)
	{
		RewardData* reward = GetRandomReward();
		if (reward == nullptr)
			return;
			

		Player* player = static_cast<Player*>(owner);
		if (player == nullptr)
			return;
		DBJobManager::GetInstance()->RewardPlayer(player , reward , _Room);
	}
}

/*---------------------------------------------------------------------------------------------
�̸�     : Monster::GetRandomReward
�뵵     : ������ Rewards ������ �� �Ѱ��� �ִ� �Լ�
������   : �̹α�
������¥ : 2022.10.24
----------------------------------------------------------------------------------------------*/
RewardData * Monster::GetRandomReward()
{
	srand((unsigned int)time(NULL));

	MonsterData* monsterdata = DataManager::GetInstacnce()->GetMonsterData(_Monsterid);
	if (monsterdata == nullptr)
		GConsoleLogManager->WriteStdOut(Color::RED, L"[Monster] : %d GetRandomReward Error", _Monsterid);

	int random = rand() % 101;

	int sum = 0;

	for(RewardData * reward : monsterdata->rewards)
	{
		sum += reward->probability;

		if (random <= sum)
			return reward;
	}

	return nullptr;

}

/*---------------------------------------------------------------------------------------------
�̸�     : Monster::BroadCastMove
�뵵     : ������ Move ���¸� �����ִ� �Լ�
������   : �̹α�
������¥ : 2022.09.30
----------------------------------------------------------------------------------------------*/
void Monster::BroadCastMove()
{
	Protocol::SERVER_MOVE movepacket;
	ObjectUtils::SetMovePacket(movepacket, this);
	_Room->BroadCast(ServerPacketManager::MakeSendBuffer(movepacket));
}
