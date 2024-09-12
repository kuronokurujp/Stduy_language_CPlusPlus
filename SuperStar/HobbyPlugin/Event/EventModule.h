#pragma once

#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "Event/EventManager.h"

namespace Event
{
    /// <summary>
    /// イベント用の追加モジュール
    /// </summary>
    class EventModule final : public Module::ModuleBase
    {
        HE_MODULE_GENRATE_DECLARATION(EventModule);

    public:
        EventModule() : ModuleBase(ModuleName()) {}

        const Core::Common::Handle AddEventManager(std::unique_ptr<EventManagerStrategyInterface>);
        const Bool RemoveEventManager(const Core::Common::Handle&);

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        const Bool _VStart() override final;
        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        const Bool _VRelease() override final;
        /// <summary>
        /// モジュール後更新
        /// </summary>
        const Bool _VLateUpdate(const Float32 in_fDeltaTime) override final;

    private:
        Uint32 _uAddEventMngCount = 0;
        Core::Common::CustomFixMap<Core::Common::Handle, EventManager*, 32> _mEventMng;
    };

}  // namespace Event
