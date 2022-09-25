#pragma once

/*---------------------------------------------------------------------------------------------
이름     : BufferReader
용도     : RecvBuffer을 사용할때 편리하게 읽을 수 있도록 돕는 객체
수정자   : 이민규
수정날짜 : 2022.08.22
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
이름     : BufferReader::operator>>
용도     : 버퍼에서 데이터를 읽는 부분을 연산자 오버로딩을 통해 편리하게 구축하는 함수
수정자   : 이민규
수정날짜 : 2022.08.22
----------------------------------------------------------------------------------------------*/
template <typename T>
BufferReader& BufferReader::operator>>(T& dest)
{
	dest = *reinterpret_cast<T*>(&_buffer[_Readsize]);
	_Readsize += sizeof(T);

	return *this;
}