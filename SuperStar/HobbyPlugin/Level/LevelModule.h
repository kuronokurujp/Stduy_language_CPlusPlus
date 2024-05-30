#pragma once

#include "Core/Core.h"
#include "Core/Common/Handle.h"

#include "Module/Module.h"

// 依存モジュール
#include "HobbyPlugin/Actor/ActorModule.h"
#include "HobbyEngine/Platform/PlatformModule.h"

#include "LevelManager.h"

#include <memory>

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
        /// TODO: モジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool Init() final override;

        /// <summary>
        /// TODO: モジュール終了
        /// </summary>
        /// <returns></returns>
        const Bool End() final override;

        const Bool Update(const Float32 in_deltaTime) final override;

        std::shared_ptr<Manager>& GetManager() { return this->_pLevelManager; }

    private:
        std::shared_ptr<Manager> _pLevelManager = NULL;
    };
}
