#include "pch.h"
#include "SendBuffer.h"

SendBuffer::SendBuffer(shared_ptr<SendBufferStorage> owner, BYTE* buffer, int32 size) : _owner(owner) , _buffer(buffer) , _size(size)
{
}

/*---------------------------------------------------------------------------------------------
이름     : SendBuffer::Close
용도     : 실제로 할당받은 사이즈와 사용할 사이즈를 체크하고 정상적일 경우 SendBufferStorage의
           close를 통해 기록하는 함수
수정자   : 이민규
수정날짜 : 2022.08.18
----------------------------------------------------------------------------------------------*/
void SendBuffer::Close(int32 writesize)
{
	CRASH_IF(_size < writesize);
	_writeSize += writesize;
	_owner->Close(writesize);
}





/*---------------------------------------------------------------------------------------------
이름     : SendBufferStorage::Reset
용도     : SendBufferStorage를 재사용하기 위해 관련인자 모두 초기화 하는 함수
수정자   : 이민규
수정날짜 : 2022.08.18
----------------------------------------------------------------------------------------------*/
void SendBufferStorage::Reset()
{
	_open = false;
	_writeSize = 0;
}

/*---------------------------------------------------------------------------------------------
이름     : SendBufferStorage::Open
용도     : SendBuffer에 buffer를 할당하고 반환해주는 함수
수정자   : 이민규
수정날짜 : 2022.08.18
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
이름     : SendBufferStorage::Close
용도     : buffer에 사용한 만큼 writeSize를 통해 기록하는 함수
수정자   : 이민규
수정날짜 : 2022.08.18
----------------------------------------------------------------------------------------------*/
void SendBufferStorage::Close(int32 writesize)
{
	CRASH_IF(_open == false);
	_open = false;
	_writeSize += writesize;
}






/*---------------------------------------------------------------------------------------------
이름     : SendBufferManager::Open
용도     : LSendBufferStorage가 할당이 되어 있지 않거나 크기가 부족할경우 교체해주고
           할당된 shared_SendBuffer를 반환하는 함수
수정자   : 이민규
수정날짜 : 2022.08.18
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
이름     : SendBufferManager::Pop()
용도     : buffer에 저장되어 있는 shared_SendBufferStorage를 반환 해주는 함수
           buffer가 비어있을 경우 Gnew를 통해 생성 후 반환
수정자   : 이민규
수정날짜 : 2022.08.18
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
이름     : SendBufferManager::Push
용도     : buffer에 shared_SendBufferStorage를 넣는 함수
수정자   : 이민규
수정날짜 : 2022.08.18
----------------------------------------------------------------------------------------------*/
void SendBufferManager::Push(shared_ptr<SendBufferStorage> buffer)
{
	WRITELOCK;
	_buffer.push_back(buffer);
}

/*---------------------------------------------------------------------------------------------
이름     : SendBufferManager::GlobalPush
용도     : GSendBufferManager의 buffer에 shared형태로 저장하는 함수
수정자   : 이민규
수정날짜 : 2022.08.18
----------------------------------------------------------------------------------------------*/
void SendBufferManager::GlobalPush(SendBufferStorage* buffer)
{
	GSendBufferManager->Push(shared_ptr<SendBufferStorage>(buffer, GlobalPush));
}
