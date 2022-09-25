#include "pch.h"
#include "BufferReader.h"

BufferReader::BufferReader(BYTE* buffer, uint32 size, uint32 readsize) : _buffer(buffer) , _size(size) , _Readsize(readsize)
{
}

/*---------------------------------------------------------------------------------------------
�̸�     : BufferReader::Peek
�뵵     : ���ۿ� �ִ� ���� _ReadSize�� ������ �ʰ� �����͸� �������� �Լ�
������   : �̹α�
������¥ : 2022.08.24
----------------------------------------------------------------------------------------------*/
bool BufferReader::Peek(void* buffer, uint32 len)
{
	if (GetFreeSize() < len)
		return false;

	::memcpy(buffer, &_buffer[_Readsize], len);
	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : BufferReader::ReadBuffer
�뵵     : ���ۿ��� buffer�� �����͸� �Ѱ��ִ� �Լ�
������   : �̹α�
������¥ : 2022.08.22
----------------------------------------------------------------------------------------------*/
bool BufferReader::ReadtoBuffer(void* buffer, uint32 len)
{
	if (GetFreeSize() < len)
		return false;

	::memcpy(buffer, &_buffer[_Readsize], len);
	_Readsize += len;

	return true;
}
