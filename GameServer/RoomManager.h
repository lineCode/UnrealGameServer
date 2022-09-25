#pragma once

class GameRoom;

/*---------------------------------------------------------------------------------------------
이름     : RoomManager
용도     : 서버에서 GameRoom을 Map으로 관리하는 싱글톤 객체
수정자   : 이민규
수정날짜 : 2022.09.12
----------------------------------------------------------------------------------------------*/
class RoomManager
{
public:
	static RoomManager& GetInstance();

	GameRoom* Add();
	bool Remove(int32 roomid);
	GameRoom* Find(int32 roomid);

private:
	RWLOCK_USE;
	static RoomManager* _Instance;
	Gmap<int32, GameRoom*> _Rooms;
	int32 _Roomid = 1;
};

