#pragma once

// エンジン最小インクルード
#include "Engine/MiniEngine.h"
#include "Event/EventManager.h"

// TODO: インゲームのイベントを全て宣言・定義する

#define INGAME_SHOT_EVENT_TYPE_NAME HE_STR_TEXT("InGameShotEvent")

namespace InGame
{
    /// <summary>
    /// TODO: 通常弾を発射するイベント
    /// </summary>
    class EventShotNormalBullet final : public Event::BaseEventData
    {
    public:
        EventShotNormalBullet(const Float32 in_fTimeStamp)
            : Event::BaseEventData(INGAME_SHOT_EVENT_TYPE_NAME, _szEventType, in_fTimeStamp)
        {
        }

        static Event::EventTypeStr& EventType() { return _szEventType; }

    private:
        static inline Event::EventTypeStr _szEventType = HE_STR_TEXT("ShotNormalBullet");
    };

}  // namespace InGame
