#pragma once
#include "Session.h"

/*---------------------------------------------------------------------------------------------
�̸�     : PacketSession
�뵵     : Recv�� �Ҷ� �ǹٸ��� �����Ͱ� �Դ��� üũ �� ��Ŷ�� �����ϱ� ���� ��ü
������   : �̹α�
������¥ : 2022.08.29
----------------------------------------------------------------------------------------------*/
class PacketSession : public Session
{
public:
	PacketSession();
	virtual ~PacketSession();

protected:
	virtual int32 PacketCheckRecv(BYTE* buffer, int32 datasize)  sealed;
	// ������ �������� ����
	virtual void ContentsRecv(BYTE* buffer, int32 datasize) abstract;

};	