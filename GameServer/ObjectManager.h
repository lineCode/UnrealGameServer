#pragma once
#include "GameObject.h"
#include "Player.h"
#include "Protocol.pb.h"

class Player;
class Monster;
class Projectile;

/*---------------------------------------------------------------------------------------------
�̸�     : ObjectManager
�뵵     : ������ Object���� ���� �� ���� �ϸ� Player�� Map���� �����ϴ� �̱��� ��ü
������   : �̹α�
������¥ : 2022.09.19
----------------------------------------------------------------------------------------------*/
class ObjectManager
{
public:
	static ObjectManager& GetInstance();

	bool Remove(int32 objectid);
	Player* Find(int32 objectid);

	template<typename T>
	requires std::derived_from<T, GameObject>
	T* Add(int32 type);

private:
	int32 CreateID(Protocol::ObjectType objecttype, int32 type);

private:
	RWLOCK_USE;
	static ObjectManager* _Instance;
	GhashMap<int32, Player*> _Players;

	int32 _Counter = 1;
};

/*---------------------------------------------------------------------------------------------
�̸�     : ObjectManager::Add
�뵵     : ���ʸ� ������� Object�� �����ϰ� Player�� ��� Map�� �߰��ؼ� �����ϴ� �Լ�
           Concept�� �̿��Ͽ� GameObject ��� ��ü����� ������ �Ұ�
������   : �̹α�
������¥ : 2022.09.24
----------------------------------------------------------------------------------------------*/
template<typename T>
requires std::derived_from<T , GameObject>
T * ObjectManager::Add(int32 type)
{
	T * object = Gnew<T>();

	{
		WRITELOCK;
		object->SetId(CreateID(object->GetObjectType() , type));
		object->SetType(type);
		if(object->GetObjectType() == Protocol::ObjectType::PLAYER)
		{
			auto player = reinterpret_cast<Player*>(object);
			_Players.insert({ player->GetId(), player });
		}

	}

	return object;
}
