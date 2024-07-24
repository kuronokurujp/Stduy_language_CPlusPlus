#pragma once

#include "Engine/Memory/MemoryManager.h"

/// <summary>
/// 初期化と終了が成功するか
/// </summary>
TEST_CASE("Memory Init to End")
{
    Core::Memory::Manager memoryManager;

    CHECK(memoryManager.Start(0x1000000));
    CHECK(memoryManager.Release());

    memoryManager.Reset();
}

/// <summary>
/// メモリ設定が成功するか
/// </summary>
TEST_CASE("Memory SetupMemory")
{
    Core::Memory::Manager memoryManager;
    CHECK(memoryManager.Start(0x1000000));

    // ページ確保テスト
    {
        // メモリサイズのイニシャライズ
        Core::Memory::Manager::PageSetupInfo memoryPageSetupInfoArray[] = {
            // 複数ページのサイズ
            {0, 3 * 1024 * 1024}, {1, 4 * 1024 * 1024}, {2, 2 * 1024 * 1024},
            {3, 2 * 1024 * 1024}, {4, 2 * 1024 * 1024}, {5, 3 * 1024 * 1024},
        };

        CHECK(memoryManager.SetupMemoryPage(memoryPageSetupInfoArray,
                                            HE_ARRAY_NUM(memoryPageSetupInfoArray)));
        CHECK(memoryManager.CheckAllMemoryBlock());
    }

    CHECK(memoryManager.Release());
    memoryManager.Reset();
}

/// <summary>
/// メモリ確保とメモリ解放を繰り返してもメモリ状態が正しくなるか
/// </summary>
TEST_CASE("Memory Allocate And Free")
{
    Core::Memory::Manager memoryManager;
    CHECK(memoryManager.Release());
    CHECK(memoryManager.Start(0x1000000));

    // ページ確保テスト
    Uint8 pageMax = 0;
    {
        // メモリサイズのイニシャライズ
        Core::Memory::Manager::PageSetupInfo memoryPageSetupInfoArray[] = {
            // 複数ページのサイズ
            {0, 3 * 1024 * 1024}, {1, 4 * 1024 * 1024}, {2, 2 * 1024 * 1024},
            {3, 2 * 1024 * 1024}, {4, 2 * 1024 * 1024}, {5, 3 * 1024 * 1024},
        };

        pageMax = HE_ARRAY_NUM(memoryPageSetupInfoArray);
        CHECK(memoryManager.SetupMemoryPage(memoryPageSetupInfoArray, pageMax));
        CHECK(memoryManager.CheckAllMemoryBlock());
    }

    // 確保と解放を繰り返して確保したのが残らないかテスト
    memoryManager.PrintAllMemoryInfo();
    for (Uint32 i = 0; i < 100; ++i)
    {
        Uint8 page = i % pageMax;

        // メモリの前確保がうまくいっているか
        Uint8* pPtr = static_cast<Uint8*>(
            memoryManager.HE_ALLOCATE_MEMORY(0x10000, page,
                                             Core::Memory::Manager::MinimumAlignSize));
        CHECK(pPtr);
        {
            *pPtr = static_cast<Uint8>(i);
            HE_LOG(HE_STR_TEXT("%d\n"), *pPtr);
        }

        // メモリの後確保がうまくいっているか
        Uint32* pPtr2 = static_cast<Uint32*>(
            memoryManager.HE_ALLOCATE_MEMORY_LAST(1000, 1,
                                                  Core::Memory::Manager::MinimumAlignSize));
        CHECK(pPtr2);
        {
            *pPtr2 = i + 1234;
            HE_LOG(HE_STR_TEXT("%d\n"), *pPtr2);
        }

        memoryManager.HE_FREE_MEMORY(pPtr);
        memoryManager.HE_FREE_MEMORY(pPtr2);
    }
    memoryManager.PrintAllMemoryInfo();

    CHECK(memoryManager.Release());
    memoryManager.Reset();
}

TEST_CASE("Memory New and Delete")
{
    Core::Memory::Manager memoryManager;
    CHECK(memoryManager.Start(0x1000000));

    // ページ確保テスト
    {
        // メモリサイズのイニシャライズ
        Core::Memory::Manager::PageSetupInfo memoryPageSetupInfoArray[] = {
            // 複数ページのサイズ
            {0, 3 * 1024 * 1024}, {1, 4 * 1024 * 1024}, {2, 2 * 1024 * 1024},
            {3, 2 * 1024 * 1024}, {4, 2 * 1024 * 1024}, {5, 3 * 1024 * 1024},
        };

        CHECK(memoryManager.SetupMemoryPage(memoryPageSetupInfoArray,
                                            HE_ARRAY_NUM(memoryPageSetupInfoArray)));
        CHECK(memoryManager.CheckAllMemoryBlock());
    }

    struct Data
    {
        Data() { this->i = 0; };
        Sint8 i = 0;
    };
    Data* pData = HE_NEW_ARRAY(Data, 5, 0);

    HE_DELETE_ARRAY(pData);

    // TODO new と deleteがうまくいっているかチェック
    CHECK(memoryManager.Release());
    memoryManager.Reset();
}
