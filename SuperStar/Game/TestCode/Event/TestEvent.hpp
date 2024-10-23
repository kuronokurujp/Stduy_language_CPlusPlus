#pragma once

#include "Event/EventManager.h"

namespace EventTest
{
    static const Event::EventTypeStr s_TestEventType(HE_STR_TEXT("TestEvent"));
    static const Event::EventTypeStr s_TestEventType2(HE_STR_TEXT("TestEvent2"));

    class EvtDataTextPut final : public Event::BaseEventData
    {
    public:
        EvtDataTextPut(const Event::EventTypeStr& in_rEventType)
            : Event::BaseEventData(in_rEventType, _szDataType, 0), _sCount(0)
        {
        }

    public:
        inline static Event::EventTypeStr _szDataType = HE_STR_TEXT("test");

        Sint32 _sCount = 0;
    };
}  // namespace EventTest

TEST_CASE("Event System")
{
    class TestListener final : public Event::EventListenerInterface
    {
    public:
        explicit TestListener() {}

        const Char* VName() override final { return HE_STR_TEXT("C_TestListener"); }

        Bool VHandleEvent(Event::EventDataInterfacePtr const& in_spEvent) override final
        {
            if (EventTest::EvtDataTextPut::_szDataType.Hash() == in_spEvent->VDataTypeHash())
            {
                EventTest::EvtDataTextPut* pEvtData =
                    reinterpret_cast<EventTest::EvtDataTextPut*>(in_spEvent.get());
                HE_ASSERT(pEvtData);

                pEvtData->_sCount += 1;

                HE_LOG_LINE(HE_STR_TEXT("test [%d]"), pEvtData->_sCount);
            }

            return TRUE;
        }
    };

    class TestEventManagerStrategy final : public Event::EventManagerStrategyInterface
    {
    public:
        Bool VIsEventTypeHash(const Uint64 in_ulHash)
        {
            return (EventTest::s_TestEventType.Hash() == in_ulHash);
        }
    };

    // カスタムヒープを使うのでメモリアロケーターを作成
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

    auto st = HE_MAKE_CUSTOM_UNIQUE_PTR(TestEventManagerStrategy);
    Event::EventManager eventMng(std::move(st));

    HE_LOG_LINE(HE_STR_TEXT("EventMangerTest"));

    // リスナー登録
    auto spTestListenr = HE_MAKE_CUSTOM_UNIQUE_PTR(TestListener);

    // リスナー追加は初回なので必ず成功する
    CHECK(eventMng.AddListener(std::move(spTestListenr), EventTest::s_TestEventType));

    // イベントは生成して所有権は管理側に渡す
    auto spTestEvent =
        HE_MAKE_CUSTOM_UNIQUE_PTR(EventTest::EvtDataTextPut, EventTest::s_TestEventType);
    CHECK(eventMng.QueueEvent(std::move(spTestEvent)));

    // 登録しているイベント型名から登録リスナーを出力
    // 共有ポインタなので取得したリストは解放しないとメモリが残る
    {
        Event::EventListenerList typeList;

        CHECK(eventMng.OutputListenerList(&typeList, EventTest::s_TestEventType));
        for (Event::EventListenerList::const_iterator i = typeList.begin(); i != typeList.end();
             i++)
        {
            HE_LOG_LINE(HE_STR_TEXT("%s"), (*i)->VName());
        }
    }

    while (eventMng.EmptyEvent() == FALSE)
    {
        CHECK(eventMng.Tick(Event::EventManager::EConstancs_Infinite));
    }

    // 戦略アリゴリズムで確保したヒープを解放してメモリアロケーター破棄でエラーにならないようにする
    eventMng.Release();

    CHECK(memoryManager.VRelease());
    memoryManager.Reset();
}

TEST_CASE("Event System All Remove Listener")
{
    class TestListener final : public Event::EventListenerInterface
    {
    public:
        explicit TestListener() {}

        const Char* VName() override final { return HE_STR_TEXT("C_TestListener"); }

        Bool VHandleEvent(Event::EventDataInterfacePtr const&) override final { return TRUE; }
    };

    class TestEventManagerStrategy final : public Event::EventManagerStrategyInterface
    {
    public:
        Bool VIsEventTypeHash(const Uint64 in_ulHash)
        {
            if (EventTest::s_TestEventType.Hash() == in_ulHash) return TRUE;
            if (EventTest::s_TestEventType2.Hash() == in_ulHash) return TRUE;
            return FALSE;
        }
    };

    // カスタムヒープを使うのでメモリアロケーターを作成
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

    auto st = HE_MAKE_CUSTOM_UNIQUE_PTR(TestEventManagerStrategy);
    Event::EventManager eventMng(std::move(st));

    HE_LOG_LINE(HE_STR_TEXT("EventMangerTest RemoveAllListener"));

    // リスナー登録
    // イベントタイプが違うリスナーを登録
    {
        auto spTestListenr  = HE_MAKE_CUSTOM_UNIQUE_PTR(TestListener);
        auto spTestListenr2 = HE_MAKE_CUSTOM_UNIQUE_PTR(TestListener);

        // リスナー追加は初回なので必ず成功する
        CHECK(eventMng.AddListener(std::move(spTestListenr), EventTest::s_TestEventType));
        CHECK(eventMng.AddListener(std::move(spTestListenr2), EventTest::s_TestEventType2));
    }

    // 指定タイプのリスナーを一括破棄
    CHECK(eventMng.RemoveAllListener(EventTest::s_TestEventType));
    CHECK(eventMng.RemoveAllListener(EventTest::s_TestEventType2));

    // 戦略アリゴリズムで確保したヒープを解放してメモリアロケーター破棄でエラーにならないようにする
    eventMng.Release();

    CHECK(memoryManager.VRelease());
    memoryManager.Reset();
}
