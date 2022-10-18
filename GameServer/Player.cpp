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
�̸�     : Player::LeaveGame
�뵵     : �÷��̾ �濡�� �������� �� ����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.09.19
----------------------------------------------------------------------------------------------*/
void Player::LeaveGame()
{
	DBJobManager::GetInstance()->SavePlayerStatus(this, _Room);
}
