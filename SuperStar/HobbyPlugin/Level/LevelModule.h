#pragma once

#include <memory>

#include "Core/Common/Handle.h"
#include "Core/Core.h"
#include "Module/Module.h"

// 依存モジュール
#include "HobbyEngine/Platform/PlatformModule.h"
#include "HobbyPlugin/Actor/ActorModule.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "LevelManager.h"

namespace Level
{
    class Manager;

    /// <summary>
    /// レベル用の追加モジュール
    /// </summary>
    class LevelModule final : public Module::ModuleBase<LevelModule>
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

        const Bool Update(const Float32 in_deltaTime) final override;

        std::shared_ptr<Manager>& GetManager() { return this->_pLevelManager; }

    private:
        std::shared_ptr<Manager> _pLevelManager = NULL;
    };
}  // namespace Level

MODULE_GENRATE_DECLARATION(Level::LevelModule, Level);
