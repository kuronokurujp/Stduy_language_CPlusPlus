#pragma once

#include "Engine/Common/Hash.h"
#include "Engine/Math/Vector2.h"
// エンジン最小インクルード
#include "Engine/MiniEngine.h"

// イベントのモジュール
#include "EventModule.h"

#define INGAME_SHOT_EVENT_TYPE_NAME HE_STR_TEXT("InGameShotEvent")

namespace InGame
{
    class InGameBulletManagerComponent;

    /// <summary>
    /// 弾の発射するイベント管理の拡張クラス
    /// </summary>
    class InGameShotEventManagerStrategy final : public Event::EventManagerStrategyInterface
    {
    public:
        InGameShotEventManagerStrategy()
            : _ulEventTypeHash(Core::Common::HashName(INGAME_SHOT_EVENT_TYPE_NAME))
        {
        }

        Bool VIsEventTypeHash(const Uint64 in_ulHash)
        {
            return (this->_ulEventTypeHash == in_ulHash);
        }

    private:
        Uint64 _ulEventTypeHash = 0;
    };

    /// <summary>
    /// 通常弾を発射するイベント
    /// </summary>
    class EventShotNormalBullet final : public Event::BaseEventData
    {
    public:
        EventShotNormalBullet(const Float32 in_fTimeStamp, const Core::Math::Vector2& in_rPos,
                              const Core::Math::Vector2& in_rDir,
                              const Uint32 in_uCollisionHashCode);

        static Uint32 EventTypeHash();

    public:
        Core::Math::Vector2 _pos;
        Core::Math::Vector2 _dir;
        Uint32 _uCollisionHashCode = 0;

    private:
        static inline Event::EventTypeStr s_szEventType = HE_STR_TEXT("ShotNormalBullet");
        static inline Uint32 s_uEventTypeHash           = s_szEventType.Hash();
    };
}  // namespace InGame
