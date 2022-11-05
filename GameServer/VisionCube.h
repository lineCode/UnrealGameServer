#pragma once

class GameObject;
class Player;

/*---------------------------------------------------------------------------------------------
�̸�     : VisionCube
�뵵     : �÷��̾��� �þ߰��� �ִ� GameObject�鸸 ��ȯ�ϵ��� �������ִ� ��ü
������   : �̹α�
������¥ : 2022.11.5
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

