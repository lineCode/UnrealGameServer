#pragma once
#include "Session.h"

/*---------------------------------------------------------------------------------------------
이름     : PacketSession
용도     : Recv를 할때 옳바르게 데이터가 왔는지 체크 후 패킷을 조립하기 위한 객체
수정자   : 이민규
수정날짜 : 2022.08.29
----------------------------------------------------------------------------------------------*/
class PacketSession : public Session
{
public:
	PacketSession();
	virtual ~PacketSession();

protected:
	virtual int32 PacketCheckRecv(BYTE* buffer, int32 datasize)  sealed;
	// 콘텐츠 영역에서 구현
	virtual void ContentsRecv(BYTE* buffer, int32 datasize) abstract;

};	