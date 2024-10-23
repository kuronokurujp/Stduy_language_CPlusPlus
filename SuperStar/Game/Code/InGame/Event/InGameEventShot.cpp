#include "InGameEventShot.h"

namespace InGame
{
    EventShotNormalBullet::EventShotNormalBullet(const Float32 in_fTimeStamp,
                                                 const Core::Math::Vector2& in_rPos,
                                                 const Core::Math::Vector2& in_rDir,
                                                 const Uint32 in_uCollisionHashCode)
        : Event::BaseEventData(INGAME_SHOT_EVENT_TYPE_NAME, s_szEventType, in_fTimeStamp),
          _pos(in_rPos),
          _dir(in_rDir),
          _uCollisionHashCode(in_uCollisionHashCode)
    {
    }

    Uint32 EventShotNormalBullet::EventTypeHash()
    {
        return s_uEventTypeHash;
    }

}  // namespace InGame
