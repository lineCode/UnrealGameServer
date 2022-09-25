#pragma once

/*---------------------------------------------------------------------------------------------
�̸�     : SendBuffer
�뵵     : SendBuffer�� ��� �ϴ� ��ü
           SendBufferStorage���� ���۸� �Ҵ� �޾� ����ϴ� ��ü
������   : �̹α�
������¥ : 2022.08.18
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
�̸�     : SendBufferStorage
�뵵     : SendBuffer���� �ʿ��� ��ŭ �Ҵ����ִ� ��ü
           SEND_STORAGE_SIZE ��ŭ �Ҵ�޾� ���� SendBuffer�� �޸� Ǯ���� ����ϴ� Ǯ���� ��ü
������   : �̹α�
������¥ : 2022.08.18
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
�̸�     : SendBufferManager
�뵵     : SendBuffer�� ����� �� ���� �Ѱ� ��ü
           SendBufferStorage�� Ǯ��������� �����ϰ� ������ �����忡�� LSendBufferStorage��
           ���� ����ϰų� ũ�⿡ ������ �� ��ü���ִ� ��ü 
������   : �̹α�
������¥ : 2022.08.18
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
