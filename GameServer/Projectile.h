#pragma once

#include "GameObject.h"

/*---------------------------------------------------------------------------------------------
�̸�     : Projectile
�뵵     : �������� �߻�ü ������ ������ ���� ��ü
������   : �̹α�
������¥ : 2022.09.19
----------------------------------------------------------------------------------------------*/
class Projectile : public GameObject
{
public:
	Projectile();

	void SetOwner(GameObject* owner) { _Owner = owner; }
	GameObject* GetOwner() { return _Owner; }

	bool GetDeadTime();

	
private:
	int64 _NextMoveTick = 0;
	GameObject * _Owner = nullptr;
	int64 _DeadTime = 0;
	
};

