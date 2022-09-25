#include "pch.h"
#include "SendBuffer.h"

SendBuffer::SendBuffer(shared_ptr<SendBufferStorage> owner, BYTE* buffer, int32 size) : _owner(owner) , _buffer(buffer) , _size(size)
{
}

/*---------------------------------------------------------------------------------------------
�̸�     : SendBuffer::Close
�뵵     : ������ �Ҵ���� ������� ����� ����� üũ�ϰ� �������� ��� SendBufferStorage��
           close�� ���� ����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.18
----------------------------------------------------------------------------------------------*/
void SendBuffer::Close(int32 writesize)
{
	CRASH_IF(_size < writesize);
	_writeSize += writesize;
	_owner->Close(writesize);
}





/*---------------------------------------------------------------------------------------------
�̸�     : SendBufferStorage::Reset
�뵵     : SendBufferStorage�� �����ϱ� ���� �������� ��� �ʱ�ȭ �ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.18
----------------------------------------------------------------------------------------------*/
void SendBufferStorage::Reset()
{
	_open = false;
	_writeSize = 0;
}

/*---------------------------------------------------------------------------------------------
�̸�     : SendBufferStorage::Open
�뵵     : SendBuffer�� buffer�� �Ҵ��ϰ� ��ȯ���ִ� �Լ�
������   : �̹α�
������¥ : 2022.08.18
----------------------------------------------------------------------------------------------*/
shared_ptr<SendBuffer> SendBufferStorage::Open(int32 size)
{
	CRASH_IF(SEND_STORAGE_SIZE <= size);
	CRASH_IF(_open == true);
	CRASH_IF(GetFreeSize() < size);

	_open = true;

	return ObjectPool<SendBuffer>::MakeShared(shared_from_this(), GetUseBuffer(), size);
}

/*---------------------------------------------------------------------------------------------
�̸�     : SendBufferStorage::Close
�뵵     : buffer�� ����� ��ŭ writeSize�� ���� ����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.18
----------------------------------------------------------------------------------------------*/
void SendBufferStorage::Close(int32 writesize)
{
	CRASH_IF(_open == false);
	_open = false;
	_writeSize += writesize;
}






/*---------------------------------------------------------------------------------------------
�̸�     : SendBufferManager::Open
�뵵     : LSendBufferStorage�� �Ҵ��� �Ǿ� ���� �ʰų� ũ�Ⱑ �����Ұ�� ��ü���ְ�
           �Ҵ�� shared_SendBuffer�� ��ȯ�ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.18
----------------------------------------------------------------------------------------------*/
shared_ptr<SendBuffer> SendBufferManager::Open(int32 size)
{
	if (LSendBufferStorage == nullptr)
	{
		LSendBufferStorage = Pop();
		LSendBufferStorage->Reset();
	}

	CRASH_IF(LSendBufferStorage->GetOpen() == true);

	if (LSendBufferStorage->GetFreeSize() > size)
	{
		LSendBufferStorage = Pop();
		LSendBufferStorage->Reset();
	}

	return LSendBufferStorage->Open(size);
}

/*---------------------------------------------------------------------------------------------
�̸�     : SendBufferManager::Pop()
�뵵     : buffer�� ����Ǿ� �ִ� shared_SendBufferStorage�� ��ȯ ���ִ� �Լ�
           buffer�� ������� ��� Gnew�� ���� ���� �� ��ȯ
������   : �̹α�
������¥ : 2022.08.18
----------------------------------------------------------------------------------------------*/
shared_ptr<SendBufferStorage> SendBufferManager::Pop()
{
	{
		WRITELOCK;
		if(_buffer.empty() == false)
		{
			shared_ptr<SendBufferStorage> Storage = _buffer.back();
			_buffer.pop_back();
			return Storage;
		}
	}

	return shared_ptr<SendBufferStorage>(Gnew<SendBufferStorage>(), GlobalPush);
}

/*---------------------------------------------------------------------------------------------
�̸�     : SendBufferManager::Push
�뵵     : buffer�� shared_SendBufferStorage�� �ִ� �Լ�
������   : �̹α�
������¥ : 2022.08.18
----------------------------------------------------------------------------------------------*/
void SendBufferManager::Push(shared_ptr<SendBufferStorage> buffer)
{
	WRITELOCK;
	_buffer.push_back(buffer);
}

/*---------------------------------------------------------------------------------------------
�̸�     : SendBufferManager::GlobalPush
�뵵     : GSendBufferManager�� buffer�� shared���·� �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.08.18
----------------------------------------------------------------------------------------------*/
void SendBufferManager::GlobalPush(SendBufferStorage* buffer)
{
	GSendBufferManager->Push(shared_ptr<SendBufferStorage>(buffer, GlobalPush));
}
