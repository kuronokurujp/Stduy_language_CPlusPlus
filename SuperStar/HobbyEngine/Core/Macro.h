#pragma once

#include "Core/Type.h"
#include "Core/Config.h"

#ifdef _HOBBY_ENGINE_DEBUG

#include <cassert>

#endif

#ifdef _HOBBY_ENGINE_DEBUG

// ログ表示
// リリース時には無効化
#ifdef _WIN

#include <windows.h>

// 呼び出し元の変数名とかぶらないようようにめちゃくちゃな変数名にしている
#define E_LOG( str, ... ) \
{ \
        static Char c_32ga311BB[256] = {}; \
        _snwprintf_s(c_32ga311BB, 256, 256, str, __VA_ARGS__); \
        OutputDebugString(c_32ga311BB); \
        OutputDebugString(L"\n"); \
}

#else

#define E_LOG(s, ...) printf(s, __VA_ARGS__)

#endif

// アサートマクロ
#define E_ASSERT(...) assert(__VA_ARGS__)

#else

#define E_LOG(...) ((void)0)
#define E_ASSERT(...) ((void)0)

#endif

// 配列の要素数を返す
// 配列のポイントでは利用できない
#define E_ARRAY_NUM(_array_) (sizeof((_array_)) / sizeof((_array_)[0]))

// 配列の全データサイズ取得
// 配列のポイントでは利用できない
#define E_ARRAY_SIZE(_tbl_) (sizeof(_tbl_))

// コンパイル時のアサート
// exprがFALSEになると, コンパイル時にエラー
// コンパイル時に値を決定出来ない式には使えない
#define E_STATIC_ASSERT(_expr_) { Sint8 static_assert_error[(_expr_) ? 1 : 0]; (void)static_assert_error; }

// デフォルトコンストラクタを封印
#define E_CLASS_DEFAULT_CONSTRUCT_NG(_x_) \
    private: _x_() = delete;

// コピーコンストラクタを封印
#define E_CLASS_COPY_CONSTRUCT_NG(_x_) \
    private: \
        _x_(const _x_ &) = delete; \
        _x_ &operator=(const _x_ &) = delete;

// 値のmin/maxマクロ
// 上限値の制御
#define E_MAX(__src__, __max__) ((__src__) < (__max__) ? (__max__) : (__src__))
// 下限値の制御
#define E_MIN(__src__, __min__) ((__src__) < (__min__) ? (__src__) : (__min__))

// 値のループマクロ
#define E_LOOPER(__src__, __min__, __max__) \
  ((__src__) > (__max__) ? (__min__) : ((__src__) < (__min__) ? (__max__) : (__src__)))

// 値をmin/max内で丸めるマクロ
#define E_CLAMP(__src__, __min__, __max__) \
  ((__src__) > (__max__) ? (__max__) : ((__src__) < (__min__) ? (__min__) : (__src__)))

// 例外を作らないキーワード
// クラスのメソッドで絶対に例外が起きない処理ではつける
// 例外をつけないコードをコンパイラが生成して最適化になる
#define E_NOEXCEPT noexcept

