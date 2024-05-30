#pragma once

#include "Core/Core.h"
#include "Core/Common/Handle.h"

#include "Module/Module.h"

namespace Actor
{
    /// <summary>
    /// アクター用の追加モジュール
    /// </summary>
    class ActorModule final : public Module::ModuleBase<ActorModule>
    {
    public:
        /// <summary>
        /// TODO: モジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool Init() final override;

        /// <summary>
        /// TODO: モジュール終了
        /// </summary>
        /// <returns></returns>
        const Bool End() final override;
    };
}
