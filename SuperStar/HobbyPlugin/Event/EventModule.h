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

        const Core::Common::Handle AddEventManager(
            Core::Memory::UniquePtr<EventManagerStrategyInterface>);
        Bool RemoveEventManager(const Core::Common::Handle&);

        /// <summary>
        /// リスナー登録 / 解除
        /// 登録したらTRUEを返す
        /// すでに登録済みなど登録失敗したらFALSE
        /// </summary>
        Bool AddListener(EventListenerPtr const&, EventTypeStr const&);
        Bool RemoveListener(EventListenerPtr const&, EventTypeStr const&);
        // 指定タイプのリスナーを一括で破棄
        Bool RemoveAllListener(EventTypeStr const&);

        /// <summary>
        /// 実行したいイベントを設定
        /// </summary>
        Bool QueueEvent(EventDataInterfacePtr const&);

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        Bool _VStart() override final;

        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        Bool _VRelease() override final;

        /// <summary>
        /// モジュール後更新
        /// </summary>
        Bool _VLateUpdate(const Float32 in_fDeltaTime) override final;

    private:
        Uint32 _uAddEventMngCount = 0;
        Core::Common::CustomFixMap<Core::Common::Handle, EventManager*, 32> _mEventMng;
    };

}  // namespace Event
