#pragma once

// エンジン最小インクルード
#include "Engine/MiniEngine.h"
#include "Event/EventManager.h"

// TODO: インゲームのイベントを全て宣言・定義する

#define INGAME_SHOT_EVENT_TYPE_NAME HE_STR_TEXT("InGameShotEvent")

namespace InGame
{
    /// <summary>
    /// 通常弾を発射するイベント
    /// </summary>
    class EventShotNormalBullet final : public Event::BaseEventData
    {
    public:
        EventShotNormalBullet(const Float32 in_fTimeStamp, const Core::Math::Vector2& in_rPos,
                              const Core::Math::Vector2& in_rDir,
                              const Uint32 in_uCollisionHashCode)
            : Event::BaseEventData(INGAME_SHOT_EVENT_TYPE_NAME, _szEventType, in_fTimeStamp),
              _pos(in_rPos),
              _dir(in_rDir),
              _uCollisionHashCode(in_uCollisionHashCode)
        {
        }

        static Event::EventTypeStr& EventType() { return _szEventType; }

    public:
        Core::Math::Vector2 _pos;
        Core::Math::Vector2 _dir;
        Uint32 _uCollisionHashCode = 0;

    private:
        static inline Event::EventTypeStr _szEventType = HE_STR_TEXT("ShotNormalBullet");
    };

}  // namespace InGame
