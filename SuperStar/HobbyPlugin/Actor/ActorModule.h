#pragma once

#include "Engine/Common/Handle.h"
#include "Engine/Core.h"
#include "Engine/Module/Module.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "Actor/Actor.h"
#include "Actor/ActorInterface.h"
#include "Actor/ActorManager.h"
#include "Actor/Component/Component.h"

namespace Actor
{
    /// <summary>
    /// アクター用の追加モジュール
    /// </summary>
    class ActorModule final : public Module::ModuleBase
    {
        HE_MODULE_GENRATE_DECLARATION(ActorModule);

    public:
        ActorModule() : ModuleBase(ModuleName()) {}

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool _Start() final override;

        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        virtual const Bool _Release() override final;
    };
}  // namespace Actor
