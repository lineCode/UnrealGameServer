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

	void Update() override;

protected:
	virtual void UpdateIdle();
	virtual void UpdateMove();
	virtual void UpdateSkill();
	virtual void UpdateDead();

protected:
	class Player* _Target = nullptr;

	int64 _NextSearchTick = 0;
	int64 _NextMoveTick = 0;

	int32 _SearchDistance = 3000;
	int32 _ChaseDistance = 7000;
	
};

