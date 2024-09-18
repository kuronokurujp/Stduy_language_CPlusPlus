#pragma once

#include "Config.h"
#include "Str.h"
#include "Type.h"

#ifdef HE_ENGINE_DEBUG

#include <cassert>

#endif

#ifdef HE_ENGINE_DEBUG

// ログ表示
// リリース時には無効化
#ifdef HE_WIN

#include <Windows.h>

#include <string>

#define HE_LOG_MSG_SIZE (2046)
#define HE_FILE __FILEW__

// 引数が文字列で2000の文字数があると以下のエラーになる
// Expression: ("Buffer too small", 0)

// 書式指定子の更新処理
template <typename T>
void HE_LOG_UPDATE_FORMAT_STRING(std::wstring& in_szFormat, size_t& in_pos, const T& in_arg)
{
    using DecayedT = std::decay_t<T>;  // 修飾子を除去した型

    if constexpr (std::is_same_v<DecayedT, wchar_t*> || std::is_same_v<DecayedT, const wchar_t*>)
    {
        if (in_pos != std::wstring::npos)
        {
            in_szFormat.replace(in_pos, 2, L"%ls");
        }
    }
    else if constexpr (std::is_same_v<DecayedT, char*> || std::is_same_v<DecayedT, const char*>)
    {
        if (in_pos != std::wstring::npos)
        {
            in_szFormat.replace(in_pos, 2, L"%hs");
        }
    }

    // 次の書式指定子を探す
    in_pos = in_szFormat.find(L"%", in_pos + 1);
}

// 共通処理を行う関数の作成
template <typename... Args>
Bool HE_LOG_CREATE_FORMATERD_STRING(Char* out, const Char* in_szFormat, Args... in_args)
{
    std::wstring szDynamicFormat = in_szFormat;
    size_t pos                   = szDynamicFormat.find(L"%");

    // 各引数に応じてフォーマット文字列を変更する
    ((HE_LOG_UPDATE_FORMAT_STRING(szDynamicFormat, pos, in_args)), ...);

    // 変換された引数を連結してワイド文字列を作成
    try
    {
        // 変換された引数を持つ動的なフォーマット文字列で出力する
        ::_snwprintf_s(out, HE_LOG_MSG_SIZE, HE_LOG_MSG_SIZE, szDynamicFormat.c_str(), in_args...);
    }
    catch (const std::exception& e)
    {
        return FALSE;
    }

    return TRUE;
}

// 文字列のローカル変数を利用するのでwhile文で囲っている
// ログ出力(改行なし)
// format引数は必ず文字列リテラルを設定する
// 文字列型の変数を入れるとコンパイルエラーになる
// コンソールにも出力
template <typename... Args>
void HE_LOG(const Char* in_szFormat, Args... in_args)
{
    // 共通部分を関数で呼び出す

    static Char szText[HE_LOG_MSG_SIZE] = {};
    if (HE_LOG_CREATE_FORMATERD_STRING(szText, in_szFormat, in_args...) == FALSE)
    {
        return;  // エラーが発生した場合は何もしない
    }

    // コンソールなどデバッグ画面で出力
    {
        ::OutputDebugString(szText);

        // WriteConsoleWを使ってワイド文字列を直接出力する
        HANDLE hConsole = ::GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole != INVALID_HANDLE_VALUE)
        {
            DWORD written = 0;
            ::WriteConsoleW(hConsole, szText, static_cast<DWORD>(HE_STR_LEN(szText)), &written,
                            nullptr);
        }
    }
}

// ログ出力(改行をする)
// format引数は必ず文字列リテラルを設定する
// 文字列型の変数を入れるとコンパイルエラーになる
// コンソールにも出力
template <typename... Args>
void HE_LOG_LINE(const Char* in_szFormat, Args... in_args)
{
    static Char szText[HE_LOG_MSG_SIZE] = {};
    if (HE_LOG_CREATE_FORMATERD_STRING(szText, in_szFormat, in_args...) == FALSE)
    {
        return;  // エラーが発生した場合は何もしない
    }

    // コンソールなどデバッグ画面で出力
    {
        ::OutputDebugString(szText);
        ::OutputDebugString(HE_STR_TEXT("\n"));

        // WriteConsoleWを使ってワイド文字列を直接出力する
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole != INVALID_HANDLE_VALUE)
        {
            DWORD written = 0;
            ::WriteConsoleW(hConsole, szText, static_cast<DWORD>(HE_STR_LEN(szText)), &written,
                            nullptr);
            ::WriteConsoleW(hConsole, HE_STR_TEXT("\n"), 1, &written, nullptr);
        }
    }
}

// プログラムが把握する情報を付与したログ出力
// ファイルパスが長く長文になる可能性があるのでログサイズ2倍の文字列サイズを確保
// コンソールにも出力
#define HE_PG_LOG_LINE(format, ...)                                                               \
    do                                                                                            \
    {                                                                                             \
        Char c[HE_LOG_MSG_SIZE] = {};                                                             \
        HE_LOG_CREATE_FORMATERD_STRING(c, format, __VA_ARGS__);                                   \
        Char c2[HE_LOG_MSG_SIZE * 2] = {};                                                        \
        _snwprintf_s(c2, HE_LOG_MSG_SIZE * 2, HE_LOG_MSG_SIZE * 2, L"%ls:%d %ls", __FILEW__,      \
                     __LINE__, c);                                                                \
        OutputDebugString(c2);                                                                    \
        OutputDebugString(L"\n");                                                                 \
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);                                        \
        if (hConsole != INVALID_HANDLE_VALUE)                                                     \
        {                                                                                         \
            DWORD written = 0;                                                                    \
            ::WriteConsoleW(hConsole, c2, static_cast<DWORD>(HE_STR_LEN(c2)), &written, nullptr); \
            ::WriteConsoleW(hConsole, HE_STR_TEXT("\n"), 1, &written, nullptr);                   \
        }                                                                                         \
    } while (0)

#else

#define HE_FILE __FILE__
#define HE_LOG_MSG_SIZE (2046)

// ログ出力(改行なし)
#define HE_LOG(s, ...) printf(s, __VA_ARGS__)
// ログ出力(改行をする)
#define HE_LOG_LINE(s, ...) \
    printf(s, __VA_ARGS__); \
    printf("\n")

// プログラムが把握する情報を付与したログ出力
// ファイルパスが長く長文になる可能性があるのでログサイズ2倍の文字列サイズを確保
#define HE_PG_LOG_LINE(format, ...)                                                                \
    do                                                                                             \
    {                                                                                              \
        Char c[HE_LOG_MSG_SIZE] = {};                                                              \
        _snprintf_s(c, HE_LOG_MSG_SIZE, HE_LOG_MSG_SIZE, format, __VA_ARGS__);                     \
        Char c2[HE_LOG_MSG_SIZE * 2] = {};                                                         \
        _snprintf_s(c2, HE_LOG_MSG_SIZE * 2, HE_LOG_MSG_SIZE * 2, "%s:%d %s\n", HE_FILE, __LINE__, \
                    c);                                                                            \
        printf(c2);                                                                                \
    } while (0)

#endif

// アサートマクロ
#define HE_ASSERT(...) assert(__VA_ARGS__)

#else

#define HE_PG_LOG_LINE(format, ...) ((void)0)

#define HE_LOG(...) ((void)0)
#define HE_LOG_LINE(format, ...) ((void)0)

#define HE_ASSERT(...) ((void)0)

#endif

// 配列の要素数を返す
// 配列のポイントでは利用できない
#define HE_ARRAY_NUM(_array_) (sizeof((_array_)) / sizeof((_array_)[0]))

// 配列の全データサイズ取得
// 配列のポイントでは利用できない
#define HE_ARRAY_SIZE(_tbl_) (sizeof(_tbl_))

// コンパイル時のアサート
// exprがFALSEになると, コンパイル時にエラー
// コンパイル時に値を決定出来ない式には使えない
#define HE_STATIC_ASSERT(_expr_)                     \
    {                                                \
        Sint8 static_assert_error[(_expr_) ? 1 : 0]; \
        (void)static_assert_error;                   \
    }

// デフォルトコンストラクタを封印
#define HE_CLASS_DEFAULT_CONSTRUCT_NG(_x_) \
private:                                   \
    _x_() = delete;

// クラスのコピー封印
#define HE_CLASS_COPY_CONSTRUCT_NG(_x_) \
private:                                \
    _x_(_x_&)       = delete;           \
    _x_(const _x_&) = delete;

#define HE_CLASS_COPY_NG(_x_)            \
private:                                 \
    _x_(_x_&)       = delete;            \
    _x_(const _x_&) = delete;            \
                                         \
private:                                 \
    _x_& operator=(_x_&)       = delete; \
    _x_& operator=(const _x_&) = delete;

// セマンティクスコンストラクターと演算子を封印
#define HE_CLASS_MOVE_CONSTRUCT_NG(_x_) \
private:                                \
    _x_(_x_&&)       = delete;          \
    _x_(const _x_&&) = delete;

#define HE_CLASS_MOVE_NG(_x_)             \
private:                                  \
    _x_(_x_&&)       = delete;            \
    _x_(const _x_&&) = delete;            \
                                          \
private:                                  \
    _x_& operator=(_x_&&)       = delete; \
    _x_& operator=(const _x_&&) = delete;

// 値のmin/maxマクロ
// 上限値の制御
#define HE_MAX(__src__, __max__) ((__src__) < (__max__) ? (__max__) : (__src__))
// 下限値の制御
#define HE_MIN(__src__, __min__) ((__src__) < (__min__) ? (__src__) : (__min__))

// 値のループマクロ
#define HE_LOOPER(__src__, __min__, __max__) \
    ((__src__) > (__max__) ? (__min__) : ((__src__) < (__min__) ? (__max__) : (__src__)))

// 値をmin/max内で丸めるマクロ
#define HE_CLAMP(__src__, __min__, __max__) \
    ((__src__) > (__max__) ? (__max__) : ((__src__) < (__min__) ? (__min__) : (__src__)))

// 例外を作らないキーワード
// クラスのメソッドで絶対に例外が起きない処理ではつける
// 例外をつけないコードをコンパイラが生成して最適化になる
// 処理が入らないプロパティのゲッターやセッターで使うのがいいと思う
#define HE_NOEXCEPT noexcept
