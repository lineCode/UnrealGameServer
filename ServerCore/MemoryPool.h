#pragma once

enum
{
	SLIST_ALIGNMENT = 16  //SLIST_HEADER�� ����ϱ� ���ؼ��� 16������ ������ �ؾ���
};

/*---------------------------------------------------------------------------------------------
�̸�     : MemoryPool
�뵵     : �̸� �޸𸮸� �Ҵ� �޾� ���� ȣ�� �ÿ� Ȯ���� �޸𸮸� �����ϰ� ȸ���ϴ� ����� ��ü
������   : �̹α�
������¥ : 2022.08.11
----------------------------------------------------------------------------------------------*/
DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
public:
	MemoryPool(int32 size);
	~MemoryPool();
	
	void Push(void* ptr);
	void* Pop();

private:
	SLIST_HEADER _pool; // ���ڼ��� �����ϴ� ����
	int32 _size = 0;
};

