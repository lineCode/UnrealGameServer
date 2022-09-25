#include "pch.h"
#include "PacketSession.h"
#include "BufferReader.h"

PacketSession::PacketSession() 
{
	
}

PacketSession::~PacketSession()
{
}

/*---------------------------------------------------------------------------------------------
�̸�     : PacketSession::PacketCheckRecv
�뵵     : �л�Ǿ� �ִ� ��Ŷ�� Ȯ�� �� �����ϰ� ���� ContentsRecv�� ȣ���ϰ� ������ �Լ�
           ��Ŷ�� ������ �Ϸ�Ǹ� 
           ContentsRecv�� Buffer�� �ش�ũ�⸦ ������
������   : �̹α�
������¥ : 2022.08.24
----------------------------------------------------------------------------------------------*/
int32 PacketSession::PacketCheckRecv(BYTE* buffer, int32 datasize)
{
	int32 ReadLen = 0;

	while(true)
	{
		const int32 storagelen = datasize - ReadLen;

		if (storagelen < sizeof(PacketHeader))
			break;

		PacketHeader * header = reinterpret_cast<PacketHeader*>(&buffer[ReadLen]);
		if (storagelen < header->_size)
			break;

		ContentsRecv(&buffer[ReadLen] , header->_size);

		ReadLen += header->_size;
	}

	return ReadLen;
}
