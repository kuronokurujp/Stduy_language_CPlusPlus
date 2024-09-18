#pragma once

#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"

// モジュールのヘッダーファイルは全てインクルードする
namespace __MODULE_NAME__
{
    /// <summary>
    /// __MODULE_NAME__アクター用の追加モジュール
    /// </summary>
    class __MODULE_NAME__Module final : public Module::ModuleBase
    {
        HE_MODULE_GENRATE_DECLARATION(__MODULE_NAME__Module);

    public:
        __MODULE_NAME__Module() : ModuleBase(ModuleName()) {}

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        const Bool _VStart() override final;

        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        const Bool _VRelease() override final;
    };
}  // namespace __MODULE_NAME__
