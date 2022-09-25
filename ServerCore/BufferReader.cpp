#include "pch.h"
#include "BufferReader.h"

BufferReader::BufferReader(BYTE* buffer, uint32 size, uint32 readsize) : _buffer(buffer) , _size(size) , _Readsize(readsize)
{
}

/*---------------------------------------------------------------------------------------------
이름     : BufferReader::Peek
용도     : 버퍼에 있는 값을 _ReadSize를 더하지 않고 데이터만 가져오는 함수
수정자   : 이민규
수정날짜 : 2022.08.24
----------------------------------------------------------------------------------------------*/
bool BufferReader::Peek(void* buffer, uint32 len)
{
	if (GetFreeSize() < len)
		return false;

	::memcpy(buffer, &_buffer[_Readsize], len);
	return true;
}

/*---------------------------------------------------------------------------------------------
이름     : BufferReader::ReadBuffer
용도     : 버퍼에서 buffer로 데이터를 넘겨주는 함수
수정자   : 이민규
수정날짜 : 2022.08.22
----------------------------------------------------------------------------------------------*/
bool BufferReader::ReadtoBuffer(void* buffer, uint32 len)
{
	if (GetFreeSize() < len)
		return false;

	::memcpy(buffer, &_buffer[_Readsize], len);
	_Readsize += len;

	return true;
}
