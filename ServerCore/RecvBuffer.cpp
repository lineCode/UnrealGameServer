#include "pch.h"
#include "RecvBuffer.h"

/*---------------------------------------------------------------------------------------------
이름     : RecvBuffer::RecvBuffer
용도     : RecvBuffer의 생성자
		   _capacity는 Buffer의 실제크기이고 _size는 버퍼의 단위이다
수정자   : 이민규
수정날짜 : 2022.08.18
----------------------------------------------------------------------------------------------*/
RecvBuffer::RecvBuffer(int32 buffersize) : _size(buffersize)
{
	_capacity = buffersize * BUFFER_COUNT;
	_buffer.resize(_capacity);
}

RecvBuffer::~RecvBuffer()
{
}

/*---------------------------------------------------------------------------------------------
이름     : RecvBuffer::SetReadPos
용도     : Buffer의 ReadPos부분을 읽은 만큼 수정해주는 함수
수정자   : 이민규
수정날짜 : 2022.08.18
----------------------------------------------------------------------------------------------*/
bool RecvBuffer::SetReadPos(int32 numofbyte)
{
	if (numofbyte > GetDataSize())
		return false;

	_ReadPos += numofbyte;

	return true;
}

/*---------------------------------------------------------------------------------------------
이름     : RecvBuffer::SetReadPos
용도     : Buffer의 WritePos부분을 데이터를 넣은 만큼 수정해주는 함수
수정자   : 이민규
수정날짜 : 2022.08.18
----------------------------------------------------------------------------------------------*/
bool RecvBuffer::SetWritePos(int32 numofbyte)
{
	if (numofbyte > GetFreeSize())
		return false;

	_WritePos += numofbyte;

	return true;
}

/*---------------------------------------------------------------------------------------------
이름     : RecvBuffer::Clear
용도     : Buffer를 특정 조건을 검사 후 만족 시 초기화 해주는 함수
           ReadPos 과 WritePos이 똑같아 지는 경우
           Buffer의 남은 공간이 버퍼의 단위보다 작아지는 경우
           ReadPos과 WirtePos을 맨 앞쪽을 다시 가르키게 해서 Buffer를 재활용함
수정자   : 이민규
수정날짜 : 2022.08.18
----------------------------------------------------------------------------------------------*/
bool RecvBuffer::Clear()
{
	int32 DataSize = GetDataSize();

	if(DataSize == 0)
	{
		_ReadPos = _WritePos = 0;
		return true;
	}
	else
	{
		if(GetFreeSize() < _size)
		{
			::memcpy(&_buffer, &_buffer[_ReadPos], DataSize);
			_ReadPos = 0;
			_WritePos = DataSize;
		}

		return true;
	}

	return false;
}
