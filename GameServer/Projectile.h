#pragma once

#include "GameObject.h"

/*---------------------------------------------------------------------------------------------
이름     : Projectile
용도     : 서버에서 발사체 데이터 저장을 위한 객체
수정자   : 이민규
수정날짜 : 2022.09.19
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

