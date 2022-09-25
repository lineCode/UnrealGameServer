#pragma once

#include <mutex>

/*-----------------------------------------------------------------------------
이름     : Types
용도     : Type들을 사용하기 편하게 관리하는 헤더
수정자   : 이민규
수정날짜 : 2022.08.11
------------------------------------------------------------------------------*/

using BYTE = unsigned char;
using int8 = __int8;
using int16 = __int16;
using int32= __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

using UniqueLock = std::unique_lock<std::mutex>;
using LockGuard = std::lock_guard<std::mutex>;
