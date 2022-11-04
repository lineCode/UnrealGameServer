#include "pch.h"
#include "RoomManager.h"
#include "GameRoom.h"

RoomManager* RoomManager::_Instance;


/*---------------------------------------------------------------------------------------------
이름     : RoomManager::GetInstance
용도     : RoomManager를 받아오는 싱글톤 함수
수정자   : 이민규
수정날짜 : 2022.09.12
----------------------------------------------------------------------------------------------*/
RoomManager& RoomManager::GetInstance()
{
	if(_Instance == nullptr)
		_Instance = Gnew<RoomManager>();

	return *_Instance;
}

/*---------------------------------------------------------------------------------------------
이름     : RoomManager::Add
용도     : GameRoom을 Map에 추가하는 함수
수정자   : 이민규
수정날짜 : 2022.10.05
----------------------------------------------------------------------------------------------*/
shared_ptr<GameRoom> RoomManager::Add(int32 mapid , int32 cell)
{
	shared_ptr<GameRoom> gameroom = GMakeShared<GameRoom>();

	gameroom->PushAsync(&GameRoom::Init , mapid , cell);

	WRITELOCK;
	gameroom->SetRoomId(_Roomid);
	_Rooms[_Roomid] = gameroom;
	_Roomid++;

	return gameroom;
}

/*---------------------------------------------------------------------------------------------
이름     : RoomManager::Remove
용도     : GameRoom을 Map에서 제거하는 함수
수정자   : 이민규
수정날짜 : 2022.09.12
----------------------------------------------------------------------------------------------*/
bool RoomManager::Remove(int32 roomid)
{
	WRITELOCK;

	if (_Rooms.find(roomid) == _Rooms.end())
		return false;

	_Rooms.erase(roomid);

	return true;
}

/*---------------------------------------------------------------------------------------------
이름     : RoomManager::Find
용도     : GameRoom을 Map에서 찾아서 반환해주는 함수
		   없을 경우 nullptr 반환
수정자   : 이민규
수정날짜 : 2022.09.12
----------------------------------------------------------------------------------------------*/
shared_ptr<GameRoom> RoomManager::Find(int32 roomid)
{
	WRITELOCK;

	if (_Rooms.find(roomid) == _Rooms.end())
		return nullptr;

	return _Rooms[roomid];
}



