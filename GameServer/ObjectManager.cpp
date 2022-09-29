#include "pch.h"
#include "ObjectManager.h"

#include "ObjectUtils.h"
#include "Player.h"

ObjectManager* ObjectManager::_Instance = nullptr;

/*---------------------------------------------------------------------------------------------
�̸�     : ObjectManager::GetInstance
�뵵     : PlayerManager�� �޾ƿ��� �̱��� �Լ�
������   : �̹α�
������¥ : 2022.09.19
----------------------------------------------------------------------------------------------*/
ObjectManager& ObjectManager::GetInstance()
{
	if (_Instance == nullptr)
		_Instance = Gnew<ObjectManager>();

	return *_Instance;
}

/*---------------------------------------------------------------------------------------------
�̸�     : ObjectManager::Remove
�뵵     : Object�� �����ϰ� Player�� ��� Map���� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.09.19
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
�̸�     : ObjectManager::Find
�뵵     : Object�� �����ϰ� Player�� ��� Map���� ã�Ƽ� ��ȯ���ִ� �Լ�
           ���� ��� nullptr ��ȯ
������   : �̹α�
������¥ : 2022.09.19
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
�̸�     : ObjectManager::CreateID
�뵵     : ObjectType�� �°� ID�� �߱� ���ִ� �Լ�
           int32�� ��Ʈ �� [UNUSED(1)OBJECT TYPE(7)] [UNUSED(1)TYPE(7)] [ID(16)] ������� �߱�
������   : �̹α�
������¥ : 2022.09.24
----------------------------------------------------------------------------------------------*/
int32 ObjectManager::CreateID(Protocol::ObjectType objecttype , int32 type) 
{
	WRITELOCK;
	return ( static_cast<int32>(objecttype) << 24 | type << 16 | (_Counter++) );
}
