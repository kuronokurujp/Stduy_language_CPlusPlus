#pragma once

#include "Engine/Common/Hash.h"
#include "Engine/Math/Vector2.h"
#include "InGame/InGameTag.h"
// エンジン最小インクルード
#include "Engine/MiniEngine.h"

// イベントのモジュール
#include "EventModule.h"

#define INGAME_CHARACTER_EVENT_TYPE_NAME HE_STR_TEXT("InGameCharacterEvent")

namespace InGame
{
    class InGameBulletManagerComponent;

    /// <summary>
    /// キャラクターのイベント管理の拡張クラス
    /// </summary>
    class InGameCharacterEventManagerStrategy final : public Event::EventManagerStrategyInterface
    {
    public:
        InGameCharacterEventManagerStrategy()
            : _ulEventTypeHash(Core::Common::HashName(INGAME_CHARACTER_EVENT_TYPE_NAME))
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
    /// 移動
    /// </summary>
    class EventCharacterMove final : public Event::BaseEventData
    {
    public:
        EventCharacterMove(const Float32 in_fTimeStamp, const EObjectTag in_eTag,
                           const Uint32 in_uId, const Core::Math::Vector2& in_rMove);

        static Uint32 EventTypeHash();

    public:
        Core::Math::Vector2 _move;
        EObjectTag _eTag = EObjectTag_None;
        Uint32 _uId      = 0;

    private:
        static inline Event::EventTypeStr s_szEventType = HE_STR_TEXT("CharacterMove");
        static inline Uint32 s_uEventTypeHash           = s_szEventType.Hash();
    };

    /// <summary>
    /// 攻撃
    /// </summary>
    class EventCharacterAttack final : public Event::BaseEventData
    {
    public:
        EventCharacterAttack(const Float32 in_fTimeStamp, const EObjectTag in_eTag,
                             const Uint32 in_uId);

        static Uint32 EventTypeHash();

    public:
        EObjectTag _eTag = EObjectTag_None;
        Uint32 _uId      = 0;

    private:
        static inline Event::EventTypeStr s_szEventType = HE_STR_TEXT("CharacterAttack");
        static inline Uint32 s_uEventTypeHash           = s_szEventType.Hash();
    };

}  // namespace InGame
