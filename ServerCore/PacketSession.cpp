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
이름     : PacketSession::PacketCheckRecv
용도     : 분산되어 있는 패킷을 확인 후 조립하고 나서 ContentsRecv에 호출하고 보내는 함수
           패킷의 조립이 완료되면 
           ContentsRecv에 Buffer와 해당크기를 보내줌
수정자   : 이민규
수정날짜 : 2022.08.24
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
