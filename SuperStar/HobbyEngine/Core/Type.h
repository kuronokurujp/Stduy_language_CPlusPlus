#pragma once

// Windows環境かどうか
#if defined(_WIN64) || defined(_WIN86)

// コンソールかどうか
#ifdef _WINDOWS
    #define _WIN
#else
    #define _CLS
#endif

// アプリがx64(8byte)かx84(4byte)かを定義
#ifdef _WIN64

#define _X64

#else

#define _X84

#endif

#endif

// x64 / x84でのポインタを整数へキャストする型
#ifdef _X64
    typedef unsigned long long Ptr;
#else
    typedef unsigned int Ptr;
#endif

typedef signed long         Sint32;
typedef signed long long    Sint64;
typedef unsigned long       Uint32;
typedef unsigned long long  Uint64;
typedef signed short        Sint16;
typedef unsigned short      Uint16;
typedef signed char         Sint8;
typedef unsigned char       Uint8;
// WindowsではUnicode前提実装なのでwchar型にしている
#ifdef _WIN
typedef wchar_t             Char;
#else
typedef char                Char;
#endif
typedef char                Byte;
typedef bool                Bool;
typedef float               Float32;

// 定数定義
#ifndef FALSE

#define FALSE (false)

#endif

#ifndef TRUE

#define TRUE (true)

#endif

// ポインタのNULLなのでc++でのポインタNULLのnullptrに置き換える必要がある
// NULLが整数0だポインタのNULLとc++コンパイラは解釈してくれない
#ifdef NULL

#undef NULL

#endif

#ifndef NULL

#define NULL (nullptr)

#endif // NULL

