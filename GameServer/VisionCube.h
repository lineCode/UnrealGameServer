#pragma once

class GameObject;
class Player;

/*---------------------------------------------------------------------------------------------
이름     : VisionCube
용도     : 플레이어의 시야각에 있는 GameObject들만 소환하도록 설정해주는 객체
수정자   : 이민규
수정날짜 : 2022.11.5
----------------------------------------------------------------------------------------------*/
class VisionCube
{
public:
	VisionCube(Player* player);

	GhashSet<GameObject*> GatherObject();
	void Update();

private:
	Player* _Owner = nullptr;
	GhashSet<GameObject*> _PreviousObjects;
};

