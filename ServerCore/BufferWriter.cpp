#include "pch.h"
#include "BufferWriter.h"

BufferWriter::BufferWriter(BYTE* buffer, uint32 size, uint32 writesize) : _buffer(buffer) , _size(size) , _writesize(writesize)
{
}

/*---------------------------------------------------------------------------------------------
이름     : BufferWriter::WriteBuffer
용도     : 버퍼에 buffer데이터를 넣는 함수
수정자   : 이민규
수정날짜 : 2022.08.22
----------------------------------------------------------------------------------------------*/
bool BufferWriter::WriteBuffer(void* buffer, uint32 len)
{
	if (GetFreeSize() < len)
		return false;

	::memcpy(&_buffer[_writesize], buffer, len);
	_writesize += len;

	return true;
}
