#pragma once

/*-----------------------------------------------------------------------------
�̸�     : CoreMacro
�뵵     : ���� ��ü �� �Լ����� ���ϰ� ����ϱ� ���� ���
������   : �̹α�
������¥ : 2022.08.12
------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
�̸�     : Lock Macro
�뵵     : RWLOCK_USE -> ReadWriteLock ���� ��ũ��
           READLOCK   -> ReadLockGuard ��� ��ũ��
		   WRITELOCK  -> WriteLockGuard ��� ��ũ��
		   GREADLOCK  -> GThreadManager READLOCK ��� ��ũ��
		   GWRITELOCK  -> GThreadManager WRITELOCK ��� ��ũ��
������   : �̹α�
������¥ : 2022.08.12
------------------------------------------------------------------------------*/

#define RWLOCKS_USE(count) ReadWriteLock _locks[count];
#define RWLOCK_USE RWLOCKS_USE(1)
#define READLOCK_IDX(idx) ReadLockGuard readlock_##idx(_locks[idx]);
#define READLOCK READLOCK_IDX(0)
#define WRITELOCK_IDX(idx) WriteLockGuard writelock_##idx(_locks[idx]);
#define WRITELOCK WRITELOCK_IDX(0)

#define GREADLOCK ReadLockGuard readlock(GThreadManager->GetLock());
#define GWRITELOCK WriteLockGuard writelock(GThreadManager->GetLock());
/*-----------------------------------------------------------------------------
�̸�     : CRASH Macro
�뵵     : CRASH -> CRASH�� ���������� �� �� ��� 
		   CRASH_IF -> ���ǽ� CRASH�� �� �� ���
������   : �̹α�
������¥ : 2022.08.12
------------------------------------------------------------------------------*/

#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define CRASH_IF(expr)				\
{									\
	if (expr)					    \
	{								\
		CRASH("ASSERT_CRASH");		\
		__analysis_assume(expr);	\
	}								\
}