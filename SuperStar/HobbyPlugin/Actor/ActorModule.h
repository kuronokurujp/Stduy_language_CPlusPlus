#pragma once

#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "Actor/Actor.h"
#include "Actor/ActorInterface.h"
#include "Actor/ActorManager.h"
#include "Actor/Component/Component.h"
#include "Actor/Component/InputComponent.h"
#include "Actor/Component/TransformComponent.h"

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
        Bool _VStart() override final;

        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        Bool _VRelease() override final;
    };
}  // namespace Actor
