#include "InGameEventCharacter.h"

namespace InGame
{
    EventCharacterMove::EventCharacterMove(const Float32 in_fTimeStamp, const EObjectTag in_eTag,
                                           const Uint32 in_uId, const Core::Math::Vector2& in_rMove)
        : Event::BaseEventData(INGAME_CHARACTER_EVENT_TYPE_NAME, s_szEventType, in_fTimeStamp),
          _move(in_rMove),
          _eTag(in_eTag),
          _uId(in_uId)
    {
    }

    Uint32 EventCharacterMove::EventTypeHash()
    {
        return s_uEventTypeHash;
    }

    EventCharacterAttack::EventCharacterAttack(const Float32 in_fTimeStamp,
                                               const EObjectTag in_eTag, const Uint32 in_uId)
        : Event::BaseEventData(INGAME_CHARACTER_EVENT_TYPE_NAME, s_szEventType, in_fTimeStamp),
          _eTag(in_eTag),
          _uId(in_uId)

    {
    }

    Uint32 EventCharacterAttack::EventTypeHash()
    {
        return s_uEventTypeHash;
    }
}  // namespace InGame
