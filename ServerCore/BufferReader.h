#pragma once

/*---------------------------------------------------------------------------------------------
�̸�     : BufferReader
�뵵     : RecvBuffer�� ����Ҷ� ���ϰ� ���� �� �ֵ��� ���� ��ü
������   : �̹α�
������¥ : 2022.08.22
----------------------------------------------------------------------------------------------*/
class BufferReader
{
	//template <typename T>
	//friend BufferReader* operator>>(BufferReader* lhs, T& rhs);

public:
	BufferReader() {};
	BufferReader(BYTE* buffer, uint32 size, uint32 readsize = 0);
	~BufferReader(){};

	template<typename T>
	bool Peek(T* buffer) { return Peek(buffer, sizeof(T)); }
	bool Peek(void* buffer, uint32 len);

	BYTE* GetBuffer() { return _buffer; }
	uint32 GetSize() { return _size; }
	uint32 GetReadSize() { return _Readsize; }
	uint32 GetFreeSize() { return _size - _Readsize; }

	template<typename T>
	bool ReadtoBuffer(T* buffer) { return ReadtoBuffer(buffer, sizeof(T)); }
	bool ReadtoBuffer(void* buffer, uint32 len);

	template<typename T>
	BufferReader& operator>>(T& dest);

private:
	BYTE* _buffer = nullptr;
	uint32 _size = 0;
	uint32 _Readsize = 0;
};

/*---------------------------------------------------------------------------------------------
�̸�     : BufferReader::operator>>
�뵵     : ���ۿ��� �����͸� �д� �κ��� ������ �����ε��� ���� ���ϰ� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.22
----------------------------------------------------------------------------------------------*/
template <typename T>
BufferReader& BufferReader::operator>>(T& dest)
{
	dest = *reinterpret_cast<T*>(&_buffer[_Readsize]);
	_Readsize += sizeof(T);

	return *this;
}