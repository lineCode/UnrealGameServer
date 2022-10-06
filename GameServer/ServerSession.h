#pragma once
#include "PacketSession.h"

/*---------------------------------------------------------------------------------------------
이름     : ServerSession
용도     : 클라이언트와 서버의 통신에 사용되는 세션 객체
수정자   : 이민규
수정날짜 : 2022.09.12
----------------------------------------------------------------------------------------------*/
class Player;

class ServerSession : public PacketSession
{
public:
	ServerSession() {};
	~ServerSession() {};

public:
	shared_ptr<ServerSession> GetServerSession() { return static_pointer_cast<ServerSession>(shared_from_this()); }

	void SetMyPlayer(Player* player) { _MyPlayer = player; }
	Player* GetMyPlayer() const { return _MyPlayer; }

protected:
	virtual void ContentsConnect() override;
	virtual void ContentsDisConnect() override;
	virtual void ContentsSend() override;
	virtual void ContentsRecv(BYTE* buffer, int32 datasize) override;

private:
	Player* _MyPlayer = nullptr;
};

