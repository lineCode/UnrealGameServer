#pragma once

class GameRoom;

/*---------------------------------------------------------------------------------------------
이름     : RoomManager
용도     : 서버에서 GameRoom을 Map으로 관리하는 싱글톤 객체
수정자   : 이민규
수정날짜 : 2022.11.02
----------------------------------------------------------------------------------------------*/
class RoomManager : public JobQueue
{
public:
	static RoomManager& GetInstance();
	shared_ptr<GameRoom> Add(int32 mapid , int32 cell = 2000);
	bool Remove(int32 roomid);
	shared_ptr<GameRoom> Find(int32 roomid);

private:
	RWLOCK_USE;
	static RoomManager* _Instance;
	Gmap<int32, shared_ptr<GameRoom>> _Rooms;
	int32 _Roomid = 1;
};

