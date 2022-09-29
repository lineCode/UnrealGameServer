#pragma once

#include "GameObject.h"

/*---------------------------------------------------------------------------------------------
�̸�     : Monster
�뵵     : �������� ���� ������ ������ ���� ��ü
������   : �̹α�
������¥ : 2022.09.29
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

