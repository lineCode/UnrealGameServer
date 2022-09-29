#include "pch.h"
#include "GameObject.h"

GameObject::~GameObject()
{
	SetRoom(nullptr);
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameObject::SetVector
�뵵     : ���� Vector���� newvector������ �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.09.19
----------------------------------------------------------------------------------------------*/
void GameObject::SetVector(Protocol::Vector newvector)
{
	auto vector = _Info.mutable_vector();
	vector->set_x(newvector.x());
	vector->set_y(newvector.y());
	vector->set_z(newvector.z());
}

/*---------------------------------------------------------------------------------------------
�̸�     : GameObject::SetRotator
�뵵     : ���� Rotator���� newroator������ �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.09.19
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
