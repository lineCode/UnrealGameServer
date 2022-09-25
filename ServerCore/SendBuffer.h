#pragma once

/*---------------------------------------------------------------------------------------------
이름     : SendBuffer
용도     : SendBuffer를 담당 하는 객체
           SendBufferStorage에서 버퍼를 할당 받아 사용하는 객체
수정자   : 이민규
수정날짜 : 2022.08.18
----------------------------------------------------------------------------------------------*/
class SendBuffer
{
public:
	SendBuffer(shared_ptr<SendBufferStorage> owner ,  BYTE * buffer , int32 size);
	~SendBuffer() {};

	BYTE* GetBuffer() { return _buffer; }
	int32 GetSize() { return _size; }
	int32 GetWriteSize() { return _writeSize; }

	void Close(int32 writesize);

private:
	BYTE* _buffer = nullptr;
	int32 _size = 0;
	int32 _writeSize = 0;
	shared_ptr<SendBufferStorage> _owner;
};

	
/*---------------------------------------------------------------------------------------------
이름     : SendBufferStorage
용도     : SendBuffer에서 필요한 만큼 할당해주는 객체
           SEND_STORAGE_SIZE 만큼 할당받아 놓고 SendBuffer의 메모리 풀링에 사용하는 풀링용 객체
수정자   : 이민규
수정날짜 : 2022.08.18
----------------------------------------------------------------------------------------------*/
class SendBufferStorage : public enable_shared_from_this<SendBufferStorage>
{
	enum
	{
		SEND_STORAGE_SIZE = 8192
	};

public:
	SendBufferStorage() {};
	~SendBufferStorage() {};

	void Reset();
	shared_ptr<SendBuffer> Open(int32 size);
	void Close(int32 writesize);

	bool GetOpen() {return _open;}
	BYTE* GetUseBuffer() { return &_buffer[_writeSize]; }
	int32 GetFreeSize() { return _buffer.size() - _writeSize; }

private:
	::array<BYTE , SEND_STORAGE_SIZE> _buffer;
	bool _open = false;
	int32 _writeSize = 0;
};


/*---------------------------------------------------------------------------------------------
이름     : SendBufferManager
용도     : SendBuffer를 사용할 때 쓰는 총괄 객체
           SendBufferStorage를 풀링방식으로 소유하고 있으며 쓰레드에서 LSendBufferStorage를
           전부 사용하거나 크기에 부족할 때 교체해주는 객체 
수정자   : 이민규
수정날짜 : 2022.08.18
----------------------------------------------------------------------------------------------*/
class SendBufferManager
{
public:
	shared_ptr<SendBuffer> Open(int32 size);
private:
	shared_ptr<SendBufferStorage> Pop();
	void Push(shared_ptr<SendBufferStorage> buffer);

	static void GlobalPush(SendBufferStorage * buffer);

private:
	RWLOCK_USE;
	Gvector<shared_ptr<SendBufferStorage>> _buffer;
};
