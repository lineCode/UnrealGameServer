#include "pch.h"
#include "Projectile.h"

#include "GameRoom.h"
#include "ServerPacketManager.h"

Projectile::Projectile()
{
	SetObjectType(Protocol::ObjectType::PROJECTILE);
	_DeadTime = ::GetTickCount64() + 500;
}

/*---------------------------------------------------------------------------------------------
이름     : Projectile::::GetDeadTime
용도     : 발사체가 시간이 되어서 파괴 여부를 체크해주는 함수
           bool 형으로 파괴 될 시간일 경우 true 아닐경우 false를 반환
수정자   : 이민규
수정날짜 : 2022.09.23
----------------------------------------------------------------------------------------------*/
bool Projectile::GetDeadTime()
{
	if (_DeadTime <= ::GetTickCount64())
		return true;

	return false;
}

//void Projectile::Update()
//{
//	if (_Owner == nullptr && _Room == nullptr)
//		return;
//
//	if (_NextMoveTick <= ::GetTickCount64())
//		return;
//
//	_NextMoveTick = ::GetTickCount64() + 50;
//
//}
