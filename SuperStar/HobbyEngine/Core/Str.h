#pragma once

#include "Type.h"

#ifdef _WIN

#include <wchar.h>

#define E_STR_LEN(t) static_cast<Uint32>(::wcslen(t))
#define E_STR_CMP(a, b) ::wcscmp(a, b)
#define E_STR_CPY_S(dst, len, src, count) ::wcsncpy_s(dst, len, src, len <= count ? _TRUNCATE : count)
#define E_STR_STR(t01, t02) ::wcsstr(t01, t02)
#define E_STR_VSNPRINTF(dst, len, count, fmt, arg) ::_vsnwprintf_s(dst, len, count, fmt, arg)
#define E_STR_LOWER(s) ::_wcslwr(s)
#define E_STR_UPPER(s) ::_wcsupr(s)

#define E_STR_TEXT(t) L##t
#define E_STR_EMPTY L""

#else

#include <string.h>

#define E_STR_LEN(t) static_cast<Uint32>(::strlen(t))
#define E_STR_CMP(a, b) ::strcmp(a, b)
#define E_STR_CPY_S(dst, len, src, count) ::strncpy_s(dst, len, src, count)
#define E_STR_STR(t01, t02) ::strstr(t01, t02)
#define E_STR_VSNPRINTF(dst, len, count, fmt, arg) ::vsnprintf_s(dst, len, count, fmt, arg)

// TODO: 未対応
#define E_STR_LOWER(s) 
#define E_STR_UPPER(s) 

#define E_STR_TEXT(t) t
#define E_STR_EMPTY ""

#endif
