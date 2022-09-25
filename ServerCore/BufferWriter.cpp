#include "pch.h"
#include "BufferWriter.h"

BufferWriter::BufferWriter(BYTE* buffer, uint32 size, uint32 writesize) : _buffer(buffer) , _size(size) , _writesize(writesize)
{
}

/*---------------------------------------------------------------------------------------------
�̸�     : BufferWriter::WriteBuffer
�뵵     : ���ۿ� buffer�����͸� �ִ� �Լ�
������   : �̹α�
������¥ : 2022.08.22
----------------------------------------------------------------------------------------------*/
bool BufferWriter::WriteBuffer(void* buffer, uint32 len)
{
	if (GetFreeSize() < len)
		return false;

	::memcpy(&_buffer[_writesize], buffer, len);
	_writesize += len;

	return true;
}
