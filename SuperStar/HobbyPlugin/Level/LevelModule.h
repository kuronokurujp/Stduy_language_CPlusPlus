#pragma once

#include <memory>

#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"

// 依存モジュール
#include "ActorModule.h"
#include "Engine/Platform/PlatformModule.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "Level/Component/LevelUserInputReceive.h"
#include "Level/LevelManager.h"

namespace Level
{
    class Manager;

    /// <summary>
    /// レベル用の追加モジュール
    /// </summary>
    class LevelModule final : public Module::ModuleBase
    {
        HE_CLASS_COPY_NG(LevelModule);
        HE_CLASS_MOVE_NG(LevelModule);
        HE_MODULE_GENRATE_DECLARATION(LevelModule);

    public:
        LevelModule();

        std::shared_ptr<Manager>& GetManager() { return this->_pLevelManager; }
        Node& GetCurrneLevel() const { return *(this->_pLevelManager->CurrentLevel()); }

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool _Start() override final;

        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        virtual const Bool _Release() override final;

        const Bool _Update(const Float32 in_fDeltaTime) override final;

    private:
        std::shared_ptr<Manager> _pLevelManager = NULL;
    };
}  // namespace Level
