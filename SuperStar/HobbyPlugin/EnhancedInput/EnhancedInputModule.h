#pragma once

#include "Engine/Common/CustomString.h"
#include "Engine/Common/Handle.h"
#include "Engine/Math/Math.h"
#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"

namespace EnhancedInput
{
    /// <summary>
    /// インプット用の追加モジュール
    /// </summary>
    class EnhancedInputModule final : public Module::ModuleBase
    {
        HE_MODULE_GENRATE_DECLARATION(EnhancedInputModule);

    public:
        EnhancedInputModule();

        /// <summary>
        /// モジュール更新の優先度
        /// 入力を最優先にしている
        /// </summary>
        /// <returns></returns>
        const Sint32 Prioryty() const override final { return -10; }

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

}  // namespace EnhancedInput
