#pragma once
/*-----------------------------------------------------------------------------
이름     : CorePch
용도     : 외부에서 라이브러리 파일을 참조할 때 사용하는 PCH
수정자   : 이민규
수정날짜 : 2022.08.11
------------------------------------------------------------------------------*/

#include <iostream>
#include <Windows.h>
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

#include "Types.h"
#include "CoreMacro.h"
#include "CoreTLS.h"
#include "MemoryManager.h"
#include "CoreGlobal.h"
#include "MemoryPoolSTL.h"
#include "ReadWriteLock.h"
#include "ObjectPool.h"
#include "JobQueue.h"

