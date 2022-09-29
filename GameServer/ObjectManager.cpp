#include "pch.h"
#include "ObjectManager.h"

#include "ObjectUtils.h"
#include "Player.h"

ObjectManager* ObjectManager::_Instance = nullptr;

/*---------------------------------------------------------------------------------------------
이름     : ObjectManager::GetInstance
용도     : PlayerManager를 받아오는 싱글톤 함수
수정자   : 이민규
수정날짜 : 2022.09.19
----------------------------------------------------------------------------------------------*/
ObjectManager& ObjectManager::GetInstance()
{
	if (_Instance == nullptr)
		_Instance = Gnew<ObjectManager>();

	return *_Instance;
}

/*---------------------------------------------------------------------------------------------
이름     : ObjectManager::Remove
용도     : Object를 제거하고 Player일 경우 Map에서 제거하는 함수
수정자   : 이민규
수정날짜 : 2022.09.19
----------------------------------------------------------------------------------------------*/
bool ObjectManager::Remove(int32 objectid)
{
	if (ObjectUtils::GetObjectType(objectid) != Protocol::ObjectType::PLAYER)
		return false;

	WRITELOCK;
	if (_Players.contains(objectid) == false)
		return false;

	Gdelete(_Players[objectid]);

	_Players.erase(objectid);

	return true;

}

/*---------------------------------------------------------------------------------------------
이름     : ObjectManager::Find
용도     : Object를 제거하고 Player일 경우 Map에서 찾아서 반환해주는 함수
           없을 경우 nullptr 반환
수정자   : 이민규
수정날짜 : 2022.09.19
----------------------------------------------------------------------------------------------*/
Player* ObjectManager::Find(int32 objectid)
{
	if (ObjectUtils::GetObjectType(objectid) != Protocol::ObjectType::PLAYER)
		return nullptr;

	WRITELOCK;

	if (_Players.contains(objectid) == false)
		return nullptr;

	return _Players[objectid];

}

/*---------------------------------------------------------------------------------------------
이름     : ObjectManager::CreateID
용도     : ObjectType에 맞게 ID를 발급 해주는 함수
           int32의 비트 중 [UNUSED(1)OBJECT TYPE(7)] [UNUSED(1)TYPE(7)] [ID(16)] 방식으로 발급
수정자   : 이민규
수정날짜 : 2022.09.24
----------------------------------------------------------------------------------------------*/
int32 ObjectManager::CreateID(Protocol::ObjectType objecttype , int32 type) 
{
	WRITELOCK;
	return ( static_cast<int32>(objecttype) << 24 | type << 16 | (_Counter++) );
}
