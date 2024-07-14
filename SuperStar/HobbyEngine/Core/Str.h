#pragma once

#include "Type.h"

#ifdef HE_WIN

#include <wchar.h>

#define HE_STR_LEN(t) static_cast<Uint32>(::wcslen(t))
#define HE_STR_CMP(a, b) ::wcscmp(a, b)
// 第二引数にはコピー配列の要素数を設定
// wcharの配列サイズだと要素数x2の設定でコピー処理をしてオーバーフローを起こした
// https://learn.microsoft.com/ja-jp/cpp/c-runtime-library/reference/strncpy-s-strncpy-s-l-wcsncpy-s-wcsncpy-s-l-mbsncpy-s-mbsncpy-s-l?view=msvc-170
#define HE_STR_CPY_S(dst, dst_size, src, src_len) ::wcsncpy_s(dst, dst_size, src, src_len)
#define HE_STR_STR(t01, t02) ::wcsstr(t01, t02)
#define HE_STR_VSNPRINTF(dst, len, count, fmt, arg) ::_vsnwprintf_s(dst, len, count, fmt, arg)
#define HE_STR_LOWER(s) ::_wcslwr(s)
#define HE_STR_UPPER(s) ::_wcsupr(s)

#define HE_STR_TEXT(t) L##t
#define HE_STR_EMPTY L""
#define HE_STR_FORMAT_TEXT L"%ls"
#define HE_STR_FORMAT_PURE_TEXT L"%hs"

#else

#include <string.h>

#define HE_STR_LEN(t) static_cast<Uint32>(::strlen(t))
#define HE_STR_CMP(a, b) ::strcmp(a, b)

#define HE_STR_CPY_S(dst, dst_size, src, src_len) ::strncpy_s(dst, dst_size, src, src_len)
#define HE_STR_STR(t01, t02) ::strstr(t01, t02)
#define HE_STR_VSNPRINTF(dst, len, count, fmt, arg) ::vsnprintf_s(dst, len, count, fmt, arg)

// TODO: 未対応
#define HE_STR_LOWER(s)
#define HE_STR_UPPER(s)

#define HE_STR_TEXT(t) t
#define HE_STR_EMPTY ""
#define HE_STR_FORMAT_TEXT "%s"
#define HE_STR_FORMAT_PURE_TEXT "%s"

#endif

#define HE_STR_ERRNO errno_t
#define HE_STR_SUCCESS(e) e == 0 ? TRUE : FALSE
