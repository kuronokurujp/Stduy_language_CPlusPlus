#pragma once

// メモリシステムの宣言やマクロを記述
// メモリ確保・解放のマクロを定義
// メモリ管理クラスを使ったメモリ制御をラッピングしている

// メモリをページの前後から確保できる
// これはメモリ確保を繰り返してメモリが断片化して求めたサイズのメモリ確保ができない場合の対策
// メモリ確保する上で前後どちらで確保するのかをルールで決めるのがいい

#include <memory>
#include <new>

#include "Engine/Core.h"
#include "MemoryManager.h"

// プレースメントしたnewとそれに対応したdeleteを作っていないのでプレースメントしたnewを使うと警告が出るので抑制
#pragma warning(disable : 4291)

#ifdef HE_ENGINE_DEBUG

// newのオーバーロード
void* operator new(size_t in_size, Uint8 in_page, Uint8 in_alignSize,
                   Core::Memory::Manager::EAllocateLocateType in_eLocateType, const UTF8* in_pFile,
                   Uint32 in_line);

#else

// newのオーバーロード
void* operator new(size_t in_size, Uint8 in_page, Uint8 in_alignSize,
                   Core::Memory::Manager::ALLOCATE_LOCATE_TYPE in_locateType);

#endif

#ifdef HE_ENGINE_DEBUG

// new[]のオーバーロード
void* operator new[](size_t in_size, Uint8 in_page, Uint8 in_alignSize,
                     Core::Memory::Manager::EAllocateLocateType in_eLocateType,
                     const UTF8* in_pFile, Uint32 in_uLine);

#else

// new[]のオーバーロード
void* operator new[](size_t in_size, Uint8 in_page, Uint8 in_alignSize,
                     Core::Memory::Manager::ALLOCATE_LOCATE_TYPE in_locateType);

#endif


#ifdef HE_ENGINE_DEBUG

// NEWマクロ
// メモリアライメント設定版
// 通常はこちらを利用
#define HE_NEW(type, page)                              \
    new (page, Core::Memory::Manager::MinimumAlignSize, \
         Core::Memory::Manager::EAllocateLocateType_Top, __FILE__, __LINE__)(type)

// NEWの配列マクロ
// メモリアライメント設定版
// 通常はこちらを利用
#define HE_NEW_ARRAY(type, num, page)                   \
    new (page, Core::Memory::Manager::MinimumAlignSize, \
         Core::Memory::Manager::EAllocateLocateType_Top, __FILE__, __LINE__)(type[num])

// NEWマクロ
// メモリアライメント指定(アライメントはMINIMUM_ALIGN_SIZEの倍数)
// 細かなメモリ確保で使用
// アライメント指定を間違えるとバグになるので蔵人向け
#define HE_NEW_ALIENT(type, page, alignSize) \
    new (page, alignSize, Core::Memory::Manager::EAllocateLocateType_Top, __FILE__, __LINE__)(type)

// NEW配列マクロ
// メモリアライメント指定(アライメントはMINIMUM_ALIGN_SIZEの倍数)
// 細かなメモリ確保で使用
// アライメント指定を間違えるとバグになるので蔵人向け
#define HE_NEW_ARRAY_ALIENT(type, num, page, alignSize)                             \
    new (page, alignSize, Core::Memory::Manager::EAllocateLocateType_Top, __FILE__, \
         __LINE__)(type[num])

// NEWマクロ
// メモリをページの後ろから確保する
// アライメント設定版
#define HE_NEW_LAST(type, page)                         \
    new (page, Core::Memory::Manager::MinimumAlignSize, \
         Core::Memory::Manager::EAllocateLocteType_Last, __FILE__, __LINE__)(type)

// NEW配列のマクロ
// メモリをページの後ろから確保する
// アライメント設定版
#define HE_NEW_ARRAY_LAST(type, num, page)              \
    new (page, Core::Memory::Manager::MinimumAlignSize, \
         Core::Memory::Manager::EAllocateLocteType_Last, __FILE__, __LINE__)(type[num])

// NEWマクロ
// メモリをページの後ろから確保する
// メモリアライメント指定(アライメントはMINIMUM_ALIGN_SIZEの倍数)
// 細かなメモリ確保で使用
// アライメント指定を間違えるとバグになるので蔵人向け
#define HE_NEW_LAST_ALIENT(type, page, alignSize) \
    new (page, alignSize, Core::Memory::Manager::EAllocateLocteType_Last, __FILE__, __LINE__)(type)

// NEW配列マクロ
// メモリをページの後ろから確保する
// メモリアライメント指定(アライメントはMINIMUM_ALIGN_SIZEの倍数)
// 細かなメモリ確保で使用
// アライメント指定を間違えるとバグになるので蔵人向け
#define HE_NEW_ARRAY_LAST_ALIENT(type, num, page, alignSize)                        \
    new (page, alignSize, Core::Memory::Manager::EAllocateLocteType_Last, __FILE__, \
         __LINE__)(type[num])

#else

// NEWマクロ
// メモリアライメント設定版
// 通常はこちらを利用
#define HE_NEW(type, page)                                \
    new (page, Core::Memory::Manager::MINIMUM_ALIGN_SIZE, \
         Core::Memory::Manager::ALLOCATE_LOCATE_TOP)(type)

// NEWの配列マクロ
// メモリアライメント設定版
// 通常はこちらを利用
#define HE_NEW_ARRAY(type, num, page)                     \
    new (page, Core::Memory::Manager::MINIMUM_ALIGN_SIZE, \
         Core::Memory::Manager::ALLOCATE_LOCATE_TOP)(type[num])

// NEWマクロ
// メモリアライメント指定(アライメントはMINIMUM_ALIGN_SIZEの倍数)
// 細かなメモリ確保で使用
// アライメント指定を間違えるとバグになるので蔵人向け
#define HE_NEW_ALIENT(type, page, alignSize) \
    new (page, alignSize, Core::Memory::Manager::ALLOCATE_LOCATE_TOP)(type)

// NEW配列マクロ
// メモリアライメント指定(アライメントはMINIMUM_ALIGN_SIZEの倍数)
// 細かなメモリ確保で使用
// アライメント指定を間違えるとバグになるので蔵人向け
#define HE_NEW_ARRAY_ALIENT(type, num, page, alignSize) \
    new (page, alignSize, Core::Memory::Manager::ALLOCATE_LOCATE_TOP)(type[num])

// NEWマクロ
// メモリをページの後ろから確保する
// アライメント設定版
#define HE_NEW_LAST(type, page)                             \
    / new (page, Core::Memory::Manager::MINIMUM_ALIGN_SIZE, \
           Core::Memory::Manager::ALLOCATE_LOCATE_LAST)(type)

// NEW配列のマクロ
// メモリをページの後ろから確保する
// アライメント設定版
#define HE_NEW_ARRAY_LAST(type, num, page)                \
    new (page, Core::Memory::Manager::MINIMUM_ALIGN_SIZE, \
         Core::Memory::Manager::ALLOCATE_LOCATE_LAST)(type[num])

// NEWマクロ
// メモリをページの後ろから確保する
// メモリアライメント指定(アライメントはMINIMUM_ALIGN_SIZEの倍数)
// 細かなメモリ確保で使用
// アライメント指定を間違えるとバグになるので蔵人向け
#define HE_NEW_LAST_ALIENT(type, page, alignSize) \
    new (page, alignSize, Core::Memory::Manager::ALLOCATE_LOCATE_LAST)(type)

// NEW配列マクロ
// メモリをページの後ろから確保する
// メモリアライメント指定(アライメントはMINIMUM_ALIGN_SIZEの倍数)
// 細かなメモリ確保で使用
// アライメント指定を間違えるとバグになるので蔵人向け
#define HE_NEW_ARRAY_LAST_ALIENT(type, num, page, alignSize) \
    new (page, alignSize, Core::Memory::Manager::ALLOCATE_LOCATE_LAST)(type[num])

#endif

/// <summary>
/// メモリ解放
/// </summary>
/// <param name="ptr"></param>
extern void FreeMemory(void*);

// メモリ解放をラップする構造体
struct DeleterFreeMemory
{
    void operator()(void* ptr) const
    {
        FreeMemory(ptr);
    }
};

// deleteのマクロ
// NEWで確保したメモリ解放で使用
#define HE_DELETE(pPtr) FreeMemory(pPtr)

// 配列のdeleteマクロ
// NEW_ARRAYで確保したメモリを解放に使用
#define HE_DELETE_ARRAY(pPtr) FreeMemory(pPtr)

// deleteを安全する実行するためのマクロ
// ポインターチェックをしてすでに解放済みの場合でもエラーにはならないようにしている
#define HE_SAFE_DELETE(pPtr) \
    {                        \
        if (pPtr)            \
        {                    \
            FreeMemory(pPtr);   \
            (pPtr) = NULL;   \
        }                    \
    }

// 確保した配列メモリをdeleteで安全する実行するためのマクロ
// ポインターチェックをしてすでに解放済みの場合でもエラーにはならないようにしている
#define HE_SAFE_DELETE_ARRAY(pPtr) \
    {                              \
        if (pPtr)                  \
        {                          \
            FreeMemory(pPtr);       \
            (pPtr) = NULL;         \
        }                          \
    }

namespace Core
{
    namespace Memory
    {
        // 任意の型Tのスマートポインタをカスタムアロケータとデリータで作成する関数
        template <typename T, typename... Args>
        std::shared_ptr<T> MakeCustomShared(Args&&... args)
        {
            // メモリ確保
            void* mem = HE_NEW(T, 0);
            if (!mem) {
                throw std::bad_alloc();
            }

            try
            {
                // 配列の要素を構築し、shared_ptrを作成する
                return std::shared_ptr<T>(new (mem) T(std::forward<Args>(args)...), DeleterFreeMemory());
            }
            catch (...)
            {
                // コンストラクタが失敗した場合はメモリを解放する
                FreeMemory(mem);
                throw;
            }
        }
    }
}
