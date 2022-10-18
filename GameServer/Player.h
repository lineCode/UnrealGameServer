#pragma once
#include "GameObject.h"
#include "Protocol.pb.h"

class GameRoom;
class ServerSession;

/*---------------------------------------------------------------------------------------------
이름     : Player
용도     : 서버에서 플레이어 데이터 저장을 위한 객체
수정자   : 이민규
수정날짜 : 2022.09.19
----------------------------------------------------------------------------------------------*/
class Player :public  GameObject
{
public:
	Player();
	virtual ~Player() override;

	void SetSession(shared_ptr<ServerSession> session) { _Session = session; }
	shared_ptr<ServerSession> GetSession(){ return _Session; }

	void SetPlayerDbId(int32 dbid) { _playerdbid = dbid; }
	int32 GetPlayerDbId() { return _playerdbid; }

	void LeaveGame();

private:
	shared_ptr<ServerSession> _Session = nullptr;
	int32 _playerdbid = 0;
};
