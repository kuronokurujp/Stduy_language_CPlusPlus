#pragma once

#include <memory>

#include "MiniEngine.h"
#include "Module/Module.h"

// 依存モジュール
#include "ActorModule.h"
#include "Platform/PlatformModule.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "Code/Component/LevelUserInputReceive.h"
#include "Code/LevelManager.h"

namespace Level
{
    class Manager;

    /// <summary>
    /// レベル用の追加モジュール
    /// </summary>
    class LevelModule final : public Module::ModuleBase
    {
    public:
        LevelModule(const Char* in_szName) : ModuleBase(in_szName) {}

        std::shared_ptr<Manager>& GetManager() { return this->_pLevelManager; }

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

        const Bool Update(const Float32 in_fDeltaTime) final override;

    private:
        std::shared_ptr<Manager> _pLevelManager = NULL;
    };
}  // namespace Level

MODULE_GENRATE_DECLARATION(Level::LevelModule, Level);

// カレントレベルを取得
#define LEVEL_MODULE_CURRENT_LEVEL (ModuleLevel()->GetManager()->CurrentLevel())
