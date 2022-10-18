#include "pch.h"
#include "Player.h"
#include "DBJobManager.h"

Player::Player()
{
	SetObjectType(Protocol::ObjectType::PLAYER);
}

Player::~Player()
{
	SetSession(nullptr);
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
