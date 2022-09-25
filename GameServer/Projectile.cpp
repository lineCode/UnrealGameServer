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
�̸�     : Projectile::::GetDeadTime
�뵵     : �߻�ü�� �ð��� �Ǿ �ı� ���θ� üũ���ִ� �Լ�
           bool ������ �ı� �� �ð��� ��� true �ƴҰ�� false�� ��ȯ
������   : �̹α�
������¥ : 2022.09.23
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
