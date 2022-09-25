#pragma once

/*---------------------------------------------------------------------------------------------
이름     : BufferWriter
용도     : SendBuffer을 할때 편하게 데이터를 넣을 수 있도록 돕는 객체
수정자   : 이민규
수정날짜 : 2022.08.22
----------------------------------------------------------------------------------------------*/
class BufferWriter
{
public:
	BufferWriter() {};
	BufferWriter(BYTE * buffer , uint32 size , uint32 writesize = 0);
	~BufferWriter() {};

	BYTE* GetBuffer() { return _buffer; }
	uint32 GetSize() { return _size; }
	uint32 GetFreeSize() { return _size - _writesize; }
	uint32 GetWriteSize() { return _writesize; }
	

	template<typename T>
	bool WriteBuffer(T* data) { return WriteBuffer(data, sizeof(T)); }
	bool WriteBuffer(void* buffer, uint32 len);


	template<typename T>
	T* Reserver();

	template<typename T>
	BufferWriter& operator<<(const T& data);

	template<typename T>
	BufferWriter& operator<<(T && data);

	template <typename T>
	BufferWriter& WriteVarible(T data);

	template<typename T, typename... Args>
	void WriteVarible(T  data, Args... args);


private:
	BYTE* _buffer = nullptr;
	uint32 _size = 0;
	uint32 _writesize = 0;
};

/*---------------------------------------------------------------------------------------------
이름     : BufferWriter::Reserver
용도     : 버퍼에 미리 쓸 공간을 예약해두는 함수
수정자   : 이민규
수정날짜 : 2022.08.22
----------------------------------------------------------------------------------------------*/
template <typename T>
T* BufferWriter::Reserver()
{
	if (GetFreeSize() < sizeof(T))
		return nullptr;

	T* ret = reinterpret_cast<T*>(&_buffer[_writesize]);
	_writesize += sizeof(T);

	return ret;
}

/*-----------------------------------------------------------------------------------------------
이름     : BufferWriter::operator<<
용도     : 버퍼에 데이터를 넣는 부분 연산자 오버로딩을 통해 편리하게 구축하는 함수 (보편참조)
수정자   : 이민규
수정날짜 : 2022.08.24
------------------------------------------------------------------------------------------------*/
template <typename T>
BufferWriter& BufferWriter::operator<<(T&& data)
{
	using DataType = std::remove_reference_t<T>;
	*reinterpret_cast<DataType*>(&_buffer[_writesize]) = std::forward<DataType>(data);
	_writesize += sizeof(T);

	return *this;
}

/*-----------------------------------------------------------------------------------------------
이름     : BufferWriter::WriteVarible
용도     : 템플릿 가변인수들을 차례로 buffer에 넣어주는 함수
수정자   : 이민규
수정날짜 : 2022.08.23
------------------------------------------------------------------------------------------------*/
template <typename T>
BufferWriter& BufferWriter::WriteVarible(T data)
{
	*reinterpret_cast<T*>(&_buffer[_writesize]) = data;
	_writesize += sizeof(T);

	return *this;
}

/*-----------------------------------------------------------------------------------------------
이름     : BufferWriter::WriteVarible
용도     : 템플릿 가변인수들을 차례로 buffer에 넣어주는 함수
		   재귀적 호출을 통해 WriteVarible(T data)를 실행 후 끝나는 템플릿 함수
수정자   : 이민규
수정날짜 : 2022.08.23
------------------------------------------------------------------------------------------------*/
template <typename T, typename ... Args>
void BufferWriter::WriteVarible(T data, Args... args)
{
	*reinterpret_cast<T*>(&_buffer[_writesize]) = data;
	_writesize += sizeof(T);

	WriteVarible(args...);
}

