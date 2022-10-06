#pragma once
#include "PacketSession.h"

/*---------------------------------------------------------------------------------------------
�̸�     : ServerSession
�뵵     : Ŭ���̾�Ʈ�� ������ ��ſ� ���Ǵ� ���� ��ü
������   : �̹α�
������¥ : 2022.09.12
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

