#pragma once

#include "GameObject.h"

/*---------------------------------------------------------------------------------------------
이름     : Monster
용도     : 서버에서 몬스터 데이터 저장을 위한 객체
수정자   : 이민규
수정날짜 : 2022.09.29
----------------------------------------------------------------------------------------------*/
class Monster : public GameObject
{
public:
	Monster();
	~Monster() override;
	void Update() override;

	void Init(int32 monsterid);
	GetSetMaker(int32, Monsterid, _Monsterid)

protected:
	virtual void UpdateIdle();
	virtual void UpdateMove();
	virtual void UpdateSkill();
	virtual void UpdateDead();
	virtual void OnDead(GameObject* attacker) override;

	class RewardData * GetRandomReward();

	void BroadCastMove();

protected:
	class Player* _Target = nullptr;

	int64 _NextSearchTick = 0;
	int64 _NextMoveTick = 0;
	int64 _CoolTimeTick = 0;

	int32 _SearchDistance = 3000;
	int32 _ChaseDistance = 7000;
	int32 _SkillRange = 300;
	int32 _Monsterid = 0;

	shared_ptr<Job> _jobs;
};

