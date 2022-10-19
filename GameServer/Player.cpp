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
�̸�     : Player::LeaveGame
�뵵     : �÷��̾ �濡�� �������� �� ����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.09.19
----------------------------------------------------------------------------------------------*/
void Player::LeaveGame()
{
	DBJobManager::GetInstance()->SavePlayerStatus(this, _Room);
}
