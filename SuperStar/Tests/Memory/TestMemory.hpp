#pragma once

#include "Core/Memory/MemoryManager.cpp"
#include "Core/Memory/MemoryManager.h"

// シングルトンなので一つしかインスタンスが作れない
static Core::Memory::Manager s_memoryManager;

/// <summary>
/// 初期化と終了が成功するか
/// </summary>
TEST_CASE("Memory Init to End")
{
    CHECK(s_memoryManager.Start(0x1000000));
    CHECK(s_memoryManager.Release());
}

/// <summary>
/// メモリ設定が成功するか
/// </summary>
TEST_CASE("Memory SetupMemory")
{
    CHECK(s_memoryManager.Release());
    CHECK(s_memoryManager.Start(0x1000000));

    // ページ確保テスト
    {
        // メモリサイズのイニシャライズ
        Core::Memory::Manager::PageSetupInfo memoryPageSetupInfoArray[] = {
            // 複数ページのサイズ
            {0, 3 * 1024 * 1024}, {1, 4 * 1024 * 1024}, {2, 2 * 1024 * 1024},
            {3, 2 * 1024 * 1024}, {4, 2 * 1024 * 1024}, {5, 3 * 1024 * 1024},
        };

        CHECK(s_memoryManager.SetupMemoryPage(memoryPageSetupInfoArray,
                                              E_ARRAY_NUM(memoryPageSetupInfoArray)));
        CHECK(s_memoryManager.CheckAllMemoryBlock());
    }

    CHECK(s_memoryManager.Release());
}

/// <summary>
/// メモリ確保とメモリ解放を繰り返してもメモリ状態が正しくなるか
/// </summary>
TEST_CASE("Memory Allocate And Free")
{
    CHECK(s_memoryManager.Release());
    CHECK(s_memoryManager.Start(0x1000000));

    // ページ確保テスト
    Uint8 pageMax = 0;
    {
        // メモリサイズのイニシャライズ
        Core::Memory::Manager::PageSetupInfo memoryPageSetupInfoArray[] = {
            // 複数ページのサイズ
            {0, 3 * 1024 * 1024}, {1, 4 * 1024 * 1024}, {2, 2 * 1024 * 1024},
            {3, 2 * 1024 * 1024}, {4, 2 * 1024 * 1024}, {5, 3 * 1024 * 1024},
        };

        pageMax = E_ARRAY_NUM(memoryPageSetupInfoArray);
        CHECK(s_memoryManager.SetupMemoryPage(memoryPageSetupInfoArray, pageMax));
        CHECK(s_memoryManager.CheckAllMemoryBlock());
    }

    // 確保と解放を繰り返して確保したのが残らないかテスト
    s_memoryManager.PrintAllMemoryInfo();
    for (Uint32 i = 0; i < 100; ++i)
    {
        Uint8 page = i % pageMax;

        // メモリの前確保がうまくいっているか
        Uint8* pPtr = static_cast<Uint8*>(
            s_memoryManager.ALLOCATE_MEMORY(0x10000, page,
                                            Core::Memory::Manager::MINIMUM_ALIGN_SIZE));
        CHECK(pPtr);
        {
            *pPtr = static_cast<Uint8>(i);
            E_LOG("%d\n", *pPtr);
        }

        // メモリの後確保がうまくいっているか
        Uint32* pPtr2 = static_cast<Uint32*>(
            s_memoryManager.ALLOCATE_MEMORY_LAST(1000, 1,
                                                 Core::Memory::Manager::MINIMUM_ALIGN_SIZE));
        CHECK(pPtr2);
        {
            *pPtr2 = i + 1234;
            E_LOG("%d\n", *pPtr2);
        }

        s_memoryManager.FREE_MEMORY(pPtr);
        s_memoryManager.FREE_MEMORY(pPtr2);
    }
    s_memoryManager.PrintAllMemoryInfo();

    CHECK(s_memoryManager.Release());
}
