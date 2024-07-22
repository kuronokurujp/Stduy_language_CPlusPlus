#pragma once

#include "Common/Handle.h"
#include "Core.h"
#include "Module/Module.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "Code/Actor.h"
#include "Code/ActorInterface.h"
#include "Code/ActorManager.h"
#include "Code/Component/Component.h"

namespace Actor
{
    /// <summary>
    /// アクター用の追加モジュール
    /// </summary>
    class ActorModule final : public Module::ModuleBase
    {
    public:
        ActorModule(const Char* in_szName) : ModuleBase(in_szName) {}

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
