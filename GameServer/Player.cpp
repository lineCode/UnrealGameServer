#include "pch.h"
#include "Player.h"
#include "Inventory.h"
#include "DBJobManager.h"

Player::Player()
{
	SetObjectType(Protocol::ObjectType::PLAYER);
	_Inventory = Gnew<Inventory>();
}

Player::~Player()
{
	SetSession(nullptr);
	Gdelete(_Inventory);
}

/*---------------------------------------------------------------------------------------------
이름     : Player::LeaveGame
용도     : 플레이어가 방에서 퇴장했을 때 사용하는 함수
수정자   : 이민규
수정날짜 : 2022.09.19
----------------------------------------------------------------------------------------------*/
void Player::LeaveGame()
{
	DBJobManager::GetInstance()->SavePlayerStatus(this, _Room);
}
