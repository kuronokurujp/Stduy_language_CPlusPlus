#pragma once

#include "Actor/Component/Component.h"
#include "Engine/MiniEngine.h"
#include "Event/EventInterface.h"

namespace InGame::Event
{
    /// <summary>
    /// イベントのリスナーコンポーネント
    /// </summary>
    class EventListenerComponent : public Actor::Component, public EventListenerInterface
    {
        HE_CLASS_COPY_NG(EventListenerComponent);
        HE_CLASS_MOVE_NG(EventListenerComponent);
        HE_GENERATED_CLASS_BODY_HEADER(EventListenerComponent, Actor::Component);

    public:
        //	コンストラクタ
        EventListenerComponent() : Actor::Component() {}
        virtual ~EventListenerComponent() {}

        // リスナー名をアスキーテキストで返す
        virtual const Char const* VName() override = 0;

        /// <summary>
        /// リスナーがイベント受け取ったかどうか
        /// </summary>
        virtual const Bool VHandleEvent(EventDataInterface const&) = 0;
    };

}  // namespace InGame::Event
