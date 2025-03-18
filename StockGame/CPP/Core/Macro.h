#pragma once
#include <windows.h>
#include "Lock.h"

/*----------------
		Lock
------------------*/

#define USE_MANY_LOCKS(count)	Lock mLocks[count];
#define USE_LOCK				USE_MANY_LOCKS(1)
#define READ_LOCK_INDEX(index)	ReadLockGuard readLockGuard_##index(mLocks[index], typeid(this).name());
#define READ_LOCK				READ_LOCK_INDEX(0)
#define WRITE_LOCK_INDEX(index)	WriteLockGuard writeLockGuard_##index(mLocks[index], typeid(this).name());
#define WRITE_LOCK				WRITE_LOCK_INDEX(0)

/*----------------
		Crash
------------------*/

#define CRASH(cause)						\
{											\
	UINT32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define ASSERT_CRASH(expr)					\
{											\
	if(!(expr))								\
	{										\
		CRASH("ASSERT_CRASH");				\
		__analysis_assume(expr);			\
	}										\
}