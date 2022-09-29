#include "pch.h"
#include "GameObject.h"

GameObject::~GameObject()
{
	SetRoom(nullptr);
}

/*---------------------------------------------------------------------------------------------
이름     : GameObject::SetVector
용도     : 나의 Vector값을 newvector값으로 수정하는 함수
수정자   : 이민규
수정날짜 : 2022.09.19
----------------------------------------------------------------------------------------------*/
void GameObject::SetVector(Protocol::Vector newvector)
{
	auto vector = _Info.mutable_vector();
	vector->set_x(newvector.x());
	vector->set_y(newvector.y());
	vector->set_z(newvector.z());
}

/*---------------------------------------------------------------------------------------------
이름     : GameObject::SetRotator
용도     : 나의 Rotator값을 newroator값으로 수정하는 함수
수정자   : 이민규
수정날짜 : 2022.09.19
----------------------------------------------------------------------------------------------*/
void GameObject::SetRotator(Protocol::Rotator newrotator)
{
	auto rotator = _Info.mutable_rotator();
	rotator->set_pitch(newrotator.pitch());
	rotator->set_yaw(newrotator.yaw());
	rotator->set_roll(newrotator.roll());
}

void GameObject::Update()
{
}
