#pragma once

#include "Core/Core.h"
#include "Core/Common/Handle.h"

#include "Module/Module.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "ActorInterface.h"
#include "ActorManager.h"
#include "Actor.h"
#include "Component/Component.h"

namespace Actor
{
    /// <summary>
    /// アクター用の追加モジュール
    /// </summary>
    class ActorModule final : public Module::ModuleBase<ActorModule>
    {
    public:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool Init() final override;

        /// <summary>
        /// モジュール終了
        /// </summary>
        /// <returns></returns>
        const Bool End() final override;
    };
}

MODULE_GENRATE_DECLARATION(Actor::ActorModule, Actor);


