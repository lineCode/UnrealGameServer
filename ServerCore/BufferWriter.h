#pragma once

/*---------------------------------------------------------------------------------------------
�̸�     : BufferWriter
�뵵     : SendBuffer�� �Ҷ� ���ϰ� �����͸� ���� �� �ֵ��� ���� ��ü
������   : �̹α�
������¥ : 2022.08.22
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
�̸�     : BufferWriter::Reserver
�뵵     : ���ۿ� �̸� �� ������ �����صδ� �Լ�
������   : �̹α�
������¥ : 2022.08.22
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
�̸�     : BufferWriter::operator<<
�뵵     : ���ۿ� �����͸� �ִ� �κ� ������ �����ε��� ���� ���ϰ� �����ϴ� �Լ� (��������)
������   : �̹α�
������¥ : 2022.08.24
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
�̸�     : BufferWriter::WriteVarible
�뵵     : ���ø� �����μ����� ���ʷ� buffer�� �־��ִ� �Լ�
������   : �̹α�
������¥ : 2022.08.23
------------------------------------------------------------------------------------------------*/
template <typename T>
BufferWriter& BufferWriter::WriteVarible(T data)
{
	*reinterpret_cast<T*>(&_buffer[_writesize]) = data;
	_writesize += sizeof(T);

	return *this;
}

/*-----------------------------------------------------------------------------------------------
�̸�     : BufferWriter::WriteVarible
�뵵     : ���ø� �����μ����� ���ʷ� buffer�� �־��ִ� �Լ�
		   ����� ȣ���� ���� WriteVarible(T data)�� ���� �� ������ ���ø� �Լ�
������   : �̹α�
������¥ : 2022.08.23
------------------------------------------------------------------------------------------------*/
template <typename T, typename ... Args>
void BufferWriter::WriteVarible(T data, Args... args)
{
	*reinterpret_cast<T*>(&_buffer[_writesize]) = data;
	_writesize += sizeof(T);

	WriteVarible(args...);
}

