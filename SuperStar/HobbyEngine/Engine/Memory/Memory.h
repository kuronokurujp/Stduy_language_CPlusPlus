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
                   Core::Memory::Manager::EAllocateLocateType in_locateType);

#endif

#ifdef HE_ENGINE_DEBUG

// new[]のオーバーロード
void* operator new[](size_t in_size, Uint8 in_page, Uint8 in_alignSize,
                     Core::Memory::Manager::EAllocateLocateType in_eLocateType,
                     const UTF8* in_pFile, Uint32 in_uLine);

#else

// new[]のオーバーロード
void* operator new[](size_t in_size, Uint8 in_page, Uint8 in_alignSize,
                     Core::Memory::Manager::EAllocateLocateType in_locateType);

#endif

#ifdef HE_ENGINE_DEBUG

// NEWマクロ
// メモリアライメント設定版
// 通常はこちらを利用
#define HE_NEW(type, page)                              \
    new (page, Core::Memory::Manager::MinimumAlignSize, \
         Core::Memory::Manager::EAllocateLocateType_Top, __FILE__, __LINE__)(type)

// マクロの引数で__FILE__と__LINE__を指定
#define HE_NEW_INFO(type, page, file, line)             \
    new (page, Core::Memory::Manager::MinimumAlignSize, \
         Core::Memory::Manager::EAllocateLocateType_Top, file, line)(type)

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
#define HE_NEW(type, page)                              \
    new (page, Core::Memory::Manager::MinimumAlignSize, \
         Core::Memory::Manager::EAllocateLocateType_Top)(type)

// NEWの配列マクロ
// メモリアライメント設定版
// 通常はこちらを利用
#define HE_NEW_ARRAY(type, num, page)                   \
    new (page, Core::Memory::Manager::MinimumAlignSize, \
         Core::Memory::Manager::EAllocateLocateType_Top)(type[num])

// NEWマクロ
// メモリアライメント指定(アライメントはMINIMUM_ALIGN_SIZEの倍数)
// 細かなメモリ確保で使用
// アライメント指定を間違えるとバグになるので蔵人向け
#define HE_NEW_ALIENT(type, page, alignSize) \
    new (page, alignSize, Core::Memory::Manager::EAllocateLocateType_Top)(type)

// NEW配列マクロ
// メモリアライメント指定(アライメントはMINIMUM_ALIGN_SIZEの倍数)
// 細かなメモリ確保で使用
// アライメント指定を間違えるとバグになるので蔵人向け
#define HE_NEW_ARRAY_ALIENT(type, num, page, alignSize) \
    new (page, alignSize, Core::Memory::Manager::EAllocateLocateType_Top)(type[num])

// NEWマクロ
// メモリをページの後ろから確保する
// アライメント設定版
#define HE_NEW_LAST(type, page)                           \
    / new (page, Core::Memory::Manager::MinimumAlignSize, \
           Core::Memory::Manager::EAllocateLocteType_Last)(type)

// NEW配列のマクロ
// メモリをページの後ろから確保する
// アライメント設定版
#define HE_NEW_ARRAY_LAST(type, num, page)              \
    new (page, Core::Memory::Manager::MinimumAlignSize, \
         Core::Memory::Manager::EAllocateLocteType_Last)(type[num])

// NEWマクロ
// メモリをページの後ろから確保する
// メモリアライメント指定(アライメントはMINIMUM_ALIGN_SIZEの倍数)
// 細かなメモリ確保で使用
// アライメント指定を間違えるとバグになるので蔵人向け
#define HE_NEW_LAST_ALIENT(type, page, alignSize) \
    new (page, alignSize, Core::Memory::Manager::EAllocateLocteType_Last)(type)

// NEW配列マクロ
// メモリをページの後ろから確保する
// メモリアライメント指定(アライメントはMINIMUM_ALIGN_SIZEの倍数)
// 細かなメモリ確保で使用
// アライメント指定を間違えるとバグになるので蔵人向け
#define HE_NEW_ARRAY_LAST_ALIENT(type, num, page, alignSize) \
    new (page, alignSize, Core::Memory::Manager::EAllocateLocteType_Last)(type[num])

#endif

/// <summary>
/// メモリ解放
/// </summary>
extern void FreeMemory(void*);

// メモリ解放をラップする構造体
struct DeleterFreeMemory
{
#ifdef HE_ENGINE_DEBUG
    const Char* szFilename = NULL;
    Uint32 uLine           = 0;

    DeleterFreeMemory(const Char* in_szFile, const Uint32 in_uLine)
        : szFilename(in_szFile), uLine(in_uLine)
    {
    }

    DeleterFreeMemory() : szFilename(HE_STR_EMPTY), uLine(0) {}
#endif

    void operator()(void* ptr) const
    {
#ifdef HE_ENGINE_DEBUG
        HE_LOG_LINE(HE_STR_TEXT("DeleteFreeMemory: %s(%d)"), this->szFilename, this->uLine);
#endif
        ::FreeMemory(ptr);
    }
};

// deleteのマクロ
// NEWで確保したメモリ解放で使用
#define HE_DELETE(pPtr) ::FreeMemory(pPtr)

// 配列のdeleteマクロ
// NEW_ARRAYで確保したメモリを解放に使用
#define HE_DELETE_ARRAY(pPtr) ::FreeMemory(pPtr)

// deleteを安全する実行するためのマクロ
// ポインターチェックをしてすでに解放済みの場合でもエラーにはならないようにしている
#define HE_SAFE_DELETE(pPtr)    \
    {                           \
        if (pPtr)               \
        {                       \
            ::FreeMemory(pPtr); \
            (pPtr) = NULL;      \
        }                       \
    }

// 確保した配列メモリをdeleteで安全する実行するためのマクロ
// ポインターチェックをしてすでに解放済みの場合でもエラーにはならないようにしている
#define HE_SAFE_DELETE_ARRAY(pPtr) \
    {                              \
        if (pPtr)                  \
        {                          \
            ::FreeMemory(pPtr);    \
            (pPtr) = NULL;         \
        }                          \
    }

namespace Core::Memory
{
    template <class T>
    using UniquePtr = std::unique_ptr<T, DeleterFreeMemory>;

    template <class T>
    using SharedPtr = std::shared_ptr<T>;

    /// <summary>
    /// 任意の型Tのスマートポインタをカスタムアロケータとデリータで作成する関数
    /// クラスの場合はデフォルトコンストラクターが必要
    /// TODO: メモリのページ指定が必要
    /// </summary>
    template <typename T, typename... Args>
#ifdef HE_ENGINE_DEBUG
    SharedPtr<T> MakeCustomSharedPtr(const Char* in_szFile, const Uint32 in_uLine, Args&&... args)
#else
    SharedPtr<T> MakeCustomSharedPtr(Args&&... args)
#endif
    {
        // 配列の要素を構築し、shared_ptrを作成する
        // 引数がない場合は配列要素を使わない
        if (sizeof...(Args) <= 0)
        {
#ifdef HE_ENGINE_DEBUG
            return SharedPtr<T>(HE_NEW(T, 0), DeleterFreeMemory(in_szFile, in_uLine));
#else
            return SharedPtr<T>(HE_NEW(T, 0), DeleterFreeMemory());
#endif
        }
        else
        {
            // メモリ確保
            // クラスだとデフォルトコンストラクタが必要
            void* pMem = HE_NEW(T, 0);
            if (pMem == NULL)
            {
                throw std::bad_alloc();
            }

            try
            {
                // 配列の要素を構築し、shared_ptrを作成する
#ifdef HE_ENGINE_DEBUG
                return SharedPtr<T>(new (pMem) T(std::forward<Args>(args)...),
                                    DeleterFreeMemory(in_szFile, in_uLine));
#else
                return SharedPtr<T>(new (pMem) T(std::forward<Args>(args)...), DeleterFreeMemory());
#endif
            }
            catch (...)
            {
                // コンストラクタが失敗した場合はメモリを解放する
                ::FreeMemory(pMem);
                throw;
            }
        }
    }

    /// <summary>
    /// std::make_unique内で独自メモリシステムを利用
    /// </summary>
    template <typename T, typename... Args>
#ifdef HE_ENGINE_DEBUG
    UniquePtr<T> MakeCustomUniquePtr(const Char* in_szFilename, Uint32 in_uLine, Args&&... args)
#else
    UniquePtr<T> MakeCustomUniquePtr(Args&&... args)
#endif
    {
        T* pMem = HE_NEW(T, 0);
        if (pMem == NULL)
        {
            throw std::bad_alloc();
        }

        if (0 < sizeof...(Args))
        {
            try
            {
                // Placement newでオブジェクトを構築
                new (pMem) T(std::forward<Args>(args)...);
            }
            catch (...)
            {
                // コンストラクタが失敗した場合はメモリを解放する
                ::FreeMemory(pMem);
                throw;
            }
        }

#ifdef HE_ENGINE_DEBUG
        return UniquePtr<T>(pMem, DeleterFreeMemory(in_szFilename, in_uLine));
#else
        return UniquePtr<T>(pMem);
#endif
    }

}  // namespace Core::Memory

// shader_ptr / unique_ptrを使うためのマクロ
// 利用箇所のファイル名とファイル行数を設定できるのでメモリリーク調査を少しでも進めるようにしている
#ifdef HE_ENGINE_DEBUG

#define HE_MAKE_CUSTOM_SHARED_PTR(T, ...) \
    Core::Memory::MakeCustomSharedPtr<T>(HE_FILE, __LINE__, ##__VA_ARGS__)
#define HE_MAKE_CUSTOM_UNIQUE_PTR(T, ...) \
    Core::Memory::MakeCustomUniquePtr<T>(HE_FILE, __LINE__, ##__VA_ARGS__)

#else

#define HE_MAKE_CUSTOM_SHARED_PTR(T, ...) Core::Memory::MakeCustomSharedPtr<T>(##__VA_ARGS__)
#define HE_MAKE_CUSTOM_UNIQUE_PTR(T, ...) Core::Memory::MakeCustomUniquePtr<T>(##__VA_ARGS__)

#endif
