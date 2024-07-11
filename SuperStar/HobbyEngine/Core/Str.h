#pragma once

#include "Type.h"

#ifdef _WIN

#include <wchar.h>

#define E_STR_LEN(t) static_cast<Uint32>(::wcslen(t))
#define E_STR_CMP(a, b) ::wcscmp(a, b)
// 第二引数にはコピー配列の要素数を設定
// wcharの配列サイズだと要素数x2の設定でコピー処理をしてオーバーフローを起こした
// https://learn.microsoft.com/ja-jp/cpp/c-runtime-library/reference/strncpy-s-strncpy-s-l-wcsncpy-s-wcsncpy-s-l-mbsncpy-s-mbsncpy-s-l?view=msvc-170
#define E_STR_CPY_S(dst, dst_size, src, src_len) ::wcsncpy_s(dst, dst_size, src, src_len)
#define E_STR_STR(t01, t02) ::wcsstr(t01, t02)
#define E_STR_VSNPRINTF(dst, len, count, fmt, arg) ::_vsnwprintf_s(dst, len, count, fmt, arg)
#define E_STR_LOWER(s) ::_wcslwr(s)
#define E_STR_UPPER(s) ::_wcsupr(s)

#define E_STR_TEXT(t) L##t
#define E_STR_EMPTY L""
#define E_STR_FORMAT_TEXT L"%ls"
#define E_STR_FORMAT_PURE_TEXT L"%hs"

#else

#include <string.h>

#define E_STR_LEN(t) static_cast<Uint32>(::strlen(t))
#define E_STR_CMP(a, b) ::strcmp(a, b)

#define E_STR_CPY_S(dst, dst_size, src, src_len) ::strncpy_s(dst, dst_size, src, src_len)
#define E_STR_STR(t01, t02) ::strstr(t01, t02)
#define E_STR_VSNPRINTF(dst, len, count, fmt, arg) ::vsnprintf_s(dst, len, count, fmt, arg)

// TODO: 未対応
#define E_STR_LOWER(s)
#define E_STR_UPPER(s)

#define E_STR_TEXT(t) t
#define E_STR_EMPTY ""
#define E_STR_FORMAT_TEXT "%s"
#define E_STR_FORMAT_PURE_TEXT "%s"

#endif

#define E_STR_ERRNO errno_t
#define E_STR_SUCCESS(e) e == 0 ? TRUE : FALSE
