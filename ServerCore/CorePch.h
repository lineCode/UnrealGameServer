#pragma once
/*-----------------------------------------------------------------------------
�̸�     : CorePch
�뵵     : �ܺο��� ���̺귯�� ������ ������ �� ����ϴ� PCH
������   : �̹α�
������¥ : 2022.08.11
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

