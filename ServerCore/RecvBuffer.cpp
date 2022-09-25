#include "pch.h"
#include "RecvBuffer.h"

/*---------------------------------------------------------------------------------------------
�̸�     : RecvBuffer::RecvBuffer
�뵵     : RecvBuffer�� ������
		   _capacity�� Buffer�� ����ũ���̰� _size�� ������ �����̴�
������   : �̹α�
������¥ : 2022.08.18
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
�̸�     : RecvBuffer::SetReadPos
�뵵     : Buffer�� ReadPos�κ��� ���� ��ŭ �������ִ� �Լ�
������   : �̹α�
������¥ : 2022.08.18
----------------------------------------------------------------------------------------------*/
bool RecvBuffer::SetReadPos(int32 numofbyte)
{
	if (numofbyte > GetDataSize())
		return false;

	_ReadPos += numofbyte;

	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : RecvBuffer::SetReadPos
�뵵     : Buffer�� WritePos�κ��� �����͸� ���� ��ŭ �������ִ� �Լ�
������   : �̹α�
������¥ : 2022.08.18
----------------------------------------------------------------------------------------------*/
bool RecvBuffer::SetWritePos(int32 numofbyte)
{
	if (numofbyte > GetFreeSize())
		return false;

	_WritePos += numofbyte;

	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�     : RecvBuffer::Clear
�뵵     : Buffer�� Ư�� ������ �˻� �� ���� �� �ʱ�ȭ ���ִ� �Լ�
           ReadPos �� WritePos�� �Ȱ��� ���� ���
           Buffer�� ���� ������ ������ �������� �۾����� ���
           ReadPos�� WirtePos�� �� ������ �ٽ� ����Ű�� �ؼ� Buffer�� ��Ȱ����
������   : �̹α�
������¥ : 2022.08.18
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
