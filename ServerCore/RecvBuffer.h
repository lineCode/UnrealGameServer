#pragma once

/*---------------------------------------------------------------------------------------------
�̸�     : RecvBuffer
�뵵     : RecvBuffer�� ����ϴ� ��ü
           Buffer�� ��� ��Ȱ�� �� �� �ֵ��� ����Ǿ� ���� 
������   : �̹α�
������¥ : 2022.08.18
----------------------------------------------------------------------------------------------*/
class RecvBuffer
{
	enum { BUFFER_COUNT = 10 };

public:
	RecvBuffer(int32 buffersize);
	~RecvBuffer();


	BYTE* GetReadBuffer() { return &_buffer[_ReadPos]; }
	BYTE* GetWriteBuffer() { return &_buffer[_WritePos]; }
	bool SetReadPos(int32 numofbyte);
	bool SetWritePos(int32 numofbyte);
	bool Clear();

	int32 GetDataSize() { return _WritePos - _ReadPos; }
	int32 GetFreeSize() { return  _capacity - _WritePos; }

private:
	int32 _ReadPos = 0;
	int32 _WritePos = 0;
	int32 _size = 0;
	int32 _capacity = 0;
	Gvector<BYTE> _buffer;
};

