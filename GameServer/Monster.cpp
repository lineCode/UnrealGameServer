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
이름     : Monster::Update
용도     : 몬스터 상태에 따라 업데이트 해주는 함수
           FSM (Finite State Machine) 머신 기본 상태
수정자   : 이민규
수정날짜 : 2022.09.29
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
이름     : Monster::UpdateIdle
용도     : 몬스터의 IDLE 상태를 업데이트 해주는 함수
           자신과 가까운 캐릭터를 찾고 찾을 경우 Target으로 지정하고 상태를 MOVE로 변경
수정자   : 이민규
수정날짜 : 2022.09.29
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
이름     : Monster::UpdateMove
용도     : 몬스터의 MOVE 상태를 업데이트 해주는 함수
		   대상과 나의 거리에서 ChaseDistance 만큼 멀어지면 취소하고
		   아니면 Target에게 이동하는 함수
수정자   : 이민규
수정날짜 : 2022.09.29
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

	// TODO : 이동
	SetVector(_Target->GetVector());

	// TODO : 다른 플레이어한테도 알려준다
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
