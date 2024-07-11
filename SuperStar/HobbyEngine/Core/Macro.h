﻿#pragma once

#include "Core/Config.h"
#include "Core/Str.h"
#include "Core/Type.h"

#ifdef _HOBBY_ENGINE_DEBUG

#include <cassert>

#endif

#ifdef _HOBBY_ENGINE_DEBUG

// ログ表示
// リリース時には無効化
#ifdef _WIN

#include <Windows.h>

#define E_LOG_MSG_SIZE (2046)
#define E_FILE __FILEW__

// 引数が文字列で2000の文字数があると以下のエラーになる
// Expression: ("Buffer too small", 0)

// 文字列のローカル変数を利用するのでwhile文で囲っている
// ログ出力(改行なし)
// format引数は必ず文字列リテラルを設定する
// 文字列型の変数を入れるとコンパイルエラーになる
#define E_LOG(format, ...)                                                    \
    do                                                                        \
    {                                                                         \
        Char c[E_LOG_MSG_SIZE] = {};                                          \
        _snwprintf_s(c, E_LOG_MSG_SIZE, E_LOG_MSG_SIZE, format, __VA_ARGS__); \
        OutputDebugString(c);                                                 \
    } while (0)

// ログ出力(改行をする)
// format引数は必ず文字列リテラルを設定する
// 文字列型の変数を入れるとコンパイルエラーになる
#define E_LOG_LINE(format, ...)                                               \
    do                                                                        \
    {                                                                         \
        Char c[E_LOG_MSG_SIZE] = {};                                          \
        _snwprintf_s(c, E_LOG_MSG_SIZE, E_LOG_MSG_SIZE, format, __VA_ARGS__); \
        OutputDebugString(c);                                                 \
        OutputDebugString(L"\n");                                             \
    } while (0)

// プログラムが把握する情報を付与したログ出力
// ファイルパスが長く長文になる可能性があるのでログサイズ2倍の文字列サイズを確保
#define E_PG_LOG_LINE(format, ...)                                                         \
    do                                                                                     \
    {                                                                                      \
        Char c[E_LOG_MSG_SIZE] = {};                                                       \
        _snwprintf_s(c, E_LOG_MSG_SIZE, E_LOG_MSG_SIZE, format, __VA_ARGS__);              \
        Char c2[E_LOG_MSG_SIZE * 2] = {};                                                  \
        _snwprintf_s(c2, E_LOG_MSG_SIZE * 2, E_LOG_MSG_SIZE * 2, L"%ls:%d %ls", __FILEW__, \
                     __LINE__, c);                                                         \
        OutputDebugString(c2);                                                             \
        OutputDebugString(L"\n");                                                          \
    } while (0)

#else

#define E_FILE __FILE__
#define E_LOG_MSG_SIZE (2046)

// ログ出力(改行なし)
#define E_LOG(s, ...) printf(s, __VA_ARGS__)
// ログ出力(改行をする)
#define E_LOG_LINE(s, ...)  \
    printf(s, __VA_ARGS__); \
    printf("\n")

// プログラムが把握する情報を付与したログ出力
// ファイルパスが長く長文になる可能性があるのでログサイズ2倍の文字列サイズを確保
#define E_PG_LOG_LINE(format, ...)                                                              \
    do                                                                                          \
    {                                                                                           \
        Char c[E_LOG_MSG_SIZE] = {};                                                            \
        _snprintf_s(c, E_LOG_MSG_SIZE, E_LOG_MSG_SIZE, format, __VA_ARGS__);                    \
        Char c2[E_LOG_MSG_SIZE * 2] = {};                                                       \
        _snprintf_s(c2, E_LOG_MSG_SIZE * 2, E_LOG_MSG_SIZE * 2, "%s:%d %s\n", E_FILE, __LINE__, \
                    c);                                                                         \
        printf(c2);                                                                             \
    } while (0)

#endif

// アサートマクロ
// テスト中はアサートで止めない
#ifndef CATCH_CONFIG_MAIN
#define E_ASSERT(...) assert(__VA_ARGS__)
#else
#define E_ASSERT(...) ((void)0)
#endif

#else

#ifdef _WIN

#include <Windows.h>

#endif

#define E_PG_LOG_LINE(format, ...) ((void)0)

#define E_LOG(...) ((void)0)
#define E_LOG_LINE(format, ...) ((void)0)

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
#define E_STATIC_ASSERT(_expr_)                      \
    {                                                \
        Sint8 static_assert_error[(_expr_) ? 1 : 0]; \
        (void)static_assert_error;                   \
    }

// デフォルトコンストラクタを封印
#define E_CLASS_DEFAULT_CONSTRUCT_NG(_x_) \
private:                                  \
    _x_() = delete;

// コピーコンストラクタを封印
#define E_CLASS_COPY_CONSTRUCT_NG(_x_)   \
private:                                 \
    _x_(_x_&)       = delete;            \
    _x_(const _x_&) = delete;            \
                                         \
private:                                 \
    _x_& operator=(_x_&)       = delete; \
    _x_& operator=(const _x_&) = delete;

// セマンティクスコンストラクターと演算子を封印
#define E_CLASS_MOVE_CONSTRUCT_NG(_x_)    \
private:                                  \
    _x_(_x_&&)       = delete;            \
    _x_(const _x_&&) = delete;            \
                                          \
private:                                  \
    _x_& operator=(_x_&&)       = delete; \
    _x_& operator=(const _x_&&) = delete;

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
// 処理が入らないプロパティのゲッターやセッターで使うのがいいと思う
#define E_NOEXCEPT noexcept
