#include "pch.h"
#include "Monster.h"
#include "Player.h"
#include "GameRoom.h"
#include "ObjectUtils.h"
#include "ServerPacketManager.h"

Monster::Monster()
{
	SetObjectType(Protocol::ObjectType::MONSTER);
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
	switch (_ObjectState)
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
			if(ObjectUtils::VectorDistance(dir) < _SearchDistance)
				return true;

			return false;
		});

	if (target == nullptr)
		return;

	_Target = target;

	_ObjectState = Protocol::ObjectState::MOVE;
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
		_ObjectState = Protocol::ObjectState::IDLE;
		return;
	}

	int32 dist = ObjectUtils::VectorDistance((_Target->GetVector() - GetVector()));
	if(dist == 0 || dist > _ChaseDistance)
	{
		_Target = nullptr;
		_ObjectState = Protocol::ObjectState::IDLE;
		return;
	}

	// TODO : �̵�
	SetVector(_Target->GetVector());

	// TODO : �ٸ� �÷��̾����׵� �˷��ش�
	Protocol::SERVER_MOVE movepacket;
	ObjectUtils::SetMovePacket(movepacket, this);
	_Room->BroadCast(ServerPacketManager::MakeSendBuffer(movepacket));
}

void Monster::UpdateSkill()
{
}

void Monster::UpdateDead()
{
}
