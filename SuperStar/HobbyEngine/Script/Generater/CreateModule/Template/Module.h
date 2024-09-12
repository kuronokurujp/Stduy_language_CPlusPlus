#pragma once

#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"

// モジュールのヘッダーファイルは全てインクルードする
namespace {MODULE_NAME}
{
    /// <summary>
    /// {MODULE_NAME}アクター用の追加モジュール
    /// </summary>
    class {MODULE_NAME}Module final : public Module::ModuleBase
    {
        HE_MODULE_GENRATE_DECLARATION({MODULE_NAME}Module);

    public:
        {MODULE_NAME}Module() : ModuleBase(ModuleName()) {}

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
} 
