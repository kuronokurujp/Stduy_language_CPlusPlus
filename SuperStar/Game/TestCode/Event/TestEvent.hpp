#pragma once

#include "Event/EventManager.h"

namespace EventTest
{
    class EvtDataTextPut final : public Event::BaseEventData
    {
    public:
        EvtDataTextPut() : _sCount(0) {}

        const Event::EventTypeStr& VEventTypeStr() const override final
        {
            return EvtDataTextPut::_szEventType;
        }

    public:
        inline static Event::EventTypeStr _szEventType = HE_STR_TEXT("test");

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

        Bool VHandleEvent(Event::EventDataInterfacePtr const& in_rEvent) override final
        {
            if (EventTest::EvtDataTextPut::_szEventType == in_rEvent->VEventTypeStr())
            {
                EventTest::EvtDataTextPut* pEvtData =
                    reinterpret_cast<EventTest::EvtDataTextPut*>(in_rEvent.get());
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
        Bool VIsEventTypeStr(const Event::EventTypeStr& in_rTypeStr)
        {
            return (EventTest::EvtDataTextPut::_szEventType == in_rTypeStr);
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
    CHECK(eventMng.VAddListenr(std::move(spTestListenr), EventTest::EvtDataTextPut::_szEventType));

    // イベントは生成して所有権は管理側に渡す
    auto spTestEvent = HE_MAKE_CUSTOM_UNIQUE_PTR(EventTest::EvtDataTextPut);
    CHECK(eventMng.VQueueEvent(std::move(spTestEvent)));

    // 登録しているイベント型名から登録リスナーを出力
    // 共有ポインタなので取得したリストは解放しないとメモリが残る
    {
        Event::EventListenerList typeList;

        CHECK(eventMng.OutputListenerList(&typeList, EventTest::EvtDataTextPut::_szEventType));
        for (Event::EventListenerList::const_iterator i = typeList.begin(); i != typeList.end();
             i++)
        {
            HE_LOG_LINE(HE_STR_TEXT("%s"), (*i)->VName());
        }
    }

    while (eventMng.EmptyEvent() == FALSE)
    {
        CHECK(eventMng.VTick(Event::EventManagerInterface::EConstancs_Infinite));
    }

    // 戦略アリゴリズムで確保したヒープを解放してメモリアロケーター破棄でエラーにならないようにする
    eventMng.VRelease();

    CHECK(memoryManager.VRelease());
    memoryManager.Reset();
}
#undef HE_USE_STD_UNIQUE_PTR
