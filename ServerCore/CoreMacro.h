#pragma once

/*-----------------------------------------------------------------------------
이름     : CoreMacro
용도     : 각종 객체 및 함수들을 편리하게 사용하기 위한 헤더
수정자   : 이민규
수정날짜 : 2022.08.12
------------------------------------------------------------------------------*/

#define NAMESPACE_BEGIN(name) namespace name {
#define NAMESPACE_END }

/*-----------------------------------------------------------------------------
이름     : Lock Macro
용도     : RWLOCK_USE -> ReadWriteLock 선언 매크로
           READLOCK   -> ReadLockGuard 사용 매크로
		   WRITELOCK  -> WriteLockGuard 사용 매크로
		   GREADLOCK  -> GThreadManager READLOCK 사용 매크로
		   GWRITELOCK  -> GThreadManager WRITELOCK 사용 매크로
수정자   : 이민규
수정날짜 : 2022.08.12
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
이름     : CRASH Macro
용도     : CRASH -> CRASH를 고의적으로 낼 때 사용 
		   CRASH_IF -> 조건식 CRASH를 낼 때 사용
수정자   : 이민규
수정날짜 : 2022.08.12
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

/*-----------------------------------------------------------------------------
이름     : GetSetMaker Macro
용도     : 함수의 GetSet을 만들어주는 매크로 (반환형 , 함수이름 , 맴버변수)
수정자   : 이민규
수정날짜 : 2022.10.19
------------------------------------------------------------------------------*/
#define GetSetMaker(returntype , funcname , valuename)\
returntype Get##funcname() {return valuename;} \
void Set##funcname(returntype data) {valuename = data;}

/*-----------------------------------------------------------------------------
이름     : ProtoGetSetMaker Macro
용도     : 프로토퍼버의 함수의 GetSet을 만들어주는 매크로
			(반환형 , 함수이름 , Get 함수명 , Set 함수명 )
수정자   : 이민규
수정날짜 : 2022.10.19
------------------------------------------------------------------------------*/
#define ProtoGetSetMaker(returntype , funcname , getvalue , setvalue)\
returntype Get##funcname() {return getvalue();} \
void Set##funcname(returntype data) {setvalue(data);} 