#include "pch.h"
#include "Player.h"


Player::Player()
{
	SetObjectType(Protocol::ObjectType::PLAYER);
}

Player::~Player()
{
	SetSession(nullptr);
}
