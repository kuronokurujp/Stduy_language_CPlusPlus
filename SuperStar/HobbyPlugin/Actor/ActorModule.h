#pragma once

#include "Core/Common/Handle.h"
#include "Core/Core.h"
#include "Module/Module.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "Actor.h"
#include "ActorInterface.h"
#include "ActorManager.h"
#include "Component/Component.h"

namespace Actor
{
    /// <summary>
    /// アクター用の追加モジュール
    /// </summary>
    class ActorModule final : public Module::ModuleBase
    {
    public:
        ActorModule(const Char* in_pName) : ModuleBase(in_pName) {}

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool Start() final override;

        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        virtual const Bool Release() override final;
    };
}  // namespace Actor

MODULE_GENRATE_DECLARATION(Actor::ActorModule, Actor);
