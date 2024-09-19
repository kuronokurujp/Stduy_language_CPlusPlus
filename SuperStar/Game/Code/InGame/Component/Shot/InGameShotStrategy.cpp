#include "InGameShotStrategy.h"

#include "EventModule.h"
#include "InGame/Event/InGameEvent.h"

namespace InGame
{
    void InGameShotStrategyNormalBullet::VConfigure(ShotConfig* out)
    {
    }

    void InGameShotStrategyNormalBullet::VShot(const ShotConfig& in_rConfig)
    {
        // TODO: 弾発生イベント通知
        auto pEventModule = Module::ModuleManager::I().Get<Event::EventModule>();

        auto spEvent = HE_MAKE_CUSTOM_SHARED_PTR(EventShotNormalBullet, 0);
        pEventModule->QueueEvent(std::move(spEvent));
    }

}  // namespace InGame
