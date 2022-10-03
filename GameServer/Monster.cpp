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

	// TODO : 스킬 사용 여부 체크
	if(dist <= _SkillRange)
	{
		_CoolTimeTick = 0;
		_State = Protocol::ObjectState::SKILL;
		return;
	}

	// TODO : 이동
	SetVector(_Target->GetVector());
}

void Monster::UpdateSkill()
{
	if(_CoolTimeTick == 0)
	{
		// TODO : 유효한 타겟인지
		if(_Target == nullptr || _Target->GetRoom() != _Room || _Target->GetHp() == 0)
		{
			_Target = nullptr;
			_State = Protocol::ObjectState::MOVE;
			BroadCastMove();
			return;
		}

		// TODO : 스킬이 아직 사용 가능한지
		Protocol::Vector dir = _Target->GetVector() - GetVector();
		int32 dist = ObjectUtils::VectorDistanceZero(dir);
		if (dist <= _SkillRange)
		{
			_State = Protocol::ObjectState::MOVE;
			BroadCastMove();
			return;
		}

		// 타겟팅 방향 주시

		// 데미지 판정

		// 스킬 사용 BroadCast

		// 스킬 쿨타임 적용
	}
}

void Monster::UpdateDead()
{
}

/*---------------------------------------------------------------------------------------------
이름     : Monster::BroadCastMove
용도     : 몬스터의 Move 상태를 보내주는 함수
수정자   : 이민규
수정날짜 : 2022.09.30
----------------------------------------------------------------------------------------------*/
void Monster::BroadCastMove()
{
	Protocol::SERVER_MOVE movepacket;
	ObjectUtils::SetMovePacket(movepacket, this);
	_Room->BroadCast(ServerPacketManager::MakeSendBuffer(movepacket));
}
