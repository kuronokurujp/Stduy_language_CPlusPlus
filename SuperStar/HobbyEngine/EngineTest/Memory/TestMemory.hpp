#pragma once

#include "Engine/Common/CustomArray.h"
#include "Engine/Memory/MemoryManager.h"

/// <summary>
/// 初期化と終了が成功するか
/// </summary>
TEST_CASE("Memory Init to End")
{
    Core::Memory::Manager memoryManager;

    CHECK(memoryManager.Start(0x1000000));
    CHECK(memoryManager.VRelease());

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

    CHECK(memoryManager.VRelease());
    memoryManager.Reset();
}

/// <summary>
/// メモリ確保とメモリ解放を繰り返してもメモリ状態が正しくなるか
/// </summary>
TEST_CASE("Memory Allocate And Free")
{
    Core::Memory::Manager memoryManager;
    CHECK(memoryManager.VRelease());
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
            HE_LOG_LINE(HE_STR_TEXT("メモリページ(%d)の前確保したメモリに設定した値(%d)"), page,
                        *pPtr);
        }

        // メモリの後確保がうまくいっているか
        Uint32* pPtr2 = static_cast<Uint32*>(
            memoryManager.HE_ALLOCATE_MEMORY_LAST(1000, 1,
                                                  Core::Memory::Manager::MinimumAlignSize));
        CHECK(pPtr2);
        {
            *pPtr2 = i + 1234;
            HE_LOG_LINE(HE_STR_TEXT("メモリページ(%d)の後確保したメモリに設定した値(%d)"), page,
                        *pPtr2);
        }

        memoryManager.HE_FREE_MEMORY(pPtr);
        memoryManager.HE_FREE_MEMORY(pPtr2);
    }
    memoryManager.PrintAllMemoryInfo();

    CHECK(memoryManager.VRelease());
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

    // メモリ確報がうまくいっているかチェック
    struct Data
    {
        Data() { this->i = 0; };
        Sint8 i = 0;
    };
    Data* pData = HE_NEW_ARRAY(Data, 5, 0);

    HE_DELETE_ARRAY(pData);

    // new と deleteがうまくいっているかチェック
    CHECK(memoryManager.VRelease());
    memoryManager.Reset();
}

TEST_CASE("Memory Custom Shader Ptr")
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

    // 共有ポインターのメモリ確報がうまくいっているかチェック
    struct Data
    {
        Data() { this->i = 0; };
        Data(Uint32 a) { this->i = a; }
        Sint8 i = 0;
    };

    Core::Common::CustomArray<std::shared_ptr<Data>, 10> memArray;
    for (Uint32 i = 0; i < 10; ++i)
    {
        auto p = HE_MAKE_CUSTOM_SHARED_PTR(Data, i);
        memArray.Set(i, p);
    }

    for (Uint32 i = 0; i < 10; ++i)
    {
        HE_LOG_LINE(HE_STR_TEXT("メモリページ(%d)の解放"), memArray[i]->i);
        CHECK(i == memArray[i]->i);
        memArray[i].reset();
    }

    // new と deleteがうまくいっているかチェック
    CHECK(memoryManager.UsedAllMemoryBlock() == FALSE);

    CHECK(memoryManager.VRelease());
    memoryManager.Reset();
}

TEST_CASE("Memory Custom Uniqe Ptr")
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

    // ユニークポインターのメモリ確保と解放がうまくいっているかチェック
    struct Data
    {
        Data() { this->i = 0; };
        Data(Uint32 a) { this->i = a; }
        Sint8 i = 0;
    };

    Core::Common::CustomArray<Core::Memory::UniquePtr<Data>, 10> memArray;
    for (Uint32 i = 0; i < 10; ++i)
    {
        auto p = HE_MAKE_CUSTOM_UNIQUE_PTR(Data, i);
        memArray.Set(i, std::move(p));
    }

    for (Uint32 i = 0; i < 10; ++i)
    {
        HE_LOG_LINE(HE_STR_TEXT("メモリページ(%d)の解放"), memArray[i]->i);
        CHECK(i == memArray[i]->i);
        memArray[i].reset();
    }

    // new と deleteがうまくいっているかチェック
    CHECK(memoryManager.UsedAllMemoryBlock() == FALSE);

    CHECK(memoryManager.VRelease());
    memoryManager.Reset();
}
