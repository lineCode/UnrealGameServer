#pragma once
#include "GameObject.h"
#include "Player.h"
#include "Protocol.pb.h"

class Player;
class Monster;
class Projectile;

/*---------------------------------------------------------------------------------------------
이름     : ObjectManager
용도     : 서버에 Object들을 생성 및 제거 하며 Player는 Map으로 관리하는 싱글톤 객체
수정자   : 이민규
수정날짜 : 2022.09.19
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
이름     : ObjectManager::Add
용도     : 제너릭 방식으로 Object를 생성하고 Player일 경우 Map에 추가해서 관리하는 함수
           Concept를 이용하여 GameObject 상속 객체말고는 생성이 불가
수정자   : 이민규
수정날짜 : 2022.09.24
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
