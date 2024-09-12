﻿#pragma once

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

        const Bool VHandleEvent(Event::EventDataInterfacePtr const& in_rEvent) override final
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
        const Bool VIsEventTypeStr(const Event::EventTypeStr& in_rTypeStr)
        {
            return (EventTest::EvtDataTextPut::_szEventType == in_rTypeStr);
        }
    };

    std::unique_ptr<Event::EventManagerStrategyInterface> st =
        std::make_unique<TestEventManagerStrategy>();
    Event::EventManager eventMng(std::move(st));

    HE_LOG_LINE(HE_STR_TEXT("EventMangerTest"));

    // リスナー登録
    auto spTestListenr = std::shared_ptr<TestListener>(new TestListener());

    // リスナー追加は初回なので必ず成功する
    CHECK(eventMng.VAddListenr(spTestListenr, EventTest::EvtDataTextPut::_szEventType));

    auto spTestEvent = std::shared_ptr<EventTest::EvtDataTextPut>(new EventTest::EvtDataTextPut());
    CHECK(eventMng.VQueueEvent(spTestEvent));

    // 登録しているイベント型名をリストを出力
    Event::EventListenerList typeList;

    CHECK(eventMng.OutputListenerList(&typeList, EventTest::EvtDataTextPut::_szEventType));
    for (Event::EventListenerList::const_iterator i = typeList.begin(); i != typeList.end(); i++)
    {
        HE_LOG_LINE(HE_STR_TEXT("%s"), (*i)->VName());
    }

    while (eventMng.EmptyEvent() == FALSE)
    {
        CHECK(eventMng.VTick(Event::EventManagerInterface::EConstancs_Infinite));
    }
}
