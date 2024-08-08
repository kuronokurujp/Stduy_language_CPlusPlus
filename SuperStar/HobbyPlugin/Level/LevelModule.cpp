#include "LevelModule.h"

#include "Engine/Memory/Memory.h"
#include "Level/LevelManager.h"

namespace Level
{
    LevelModule::LevelModule() : ModuleBase(ModuleName())
    {
        // 依存しているモジュールを設定
        // モジュールの依存設定していないと依存したモジュールが使えない
        this->_AppendDependenceModule<Level::LevelModule>();
        this->_AppendDependenceModule<Actor::ActorModule>();
        this->_AppendDependenceModule<Platform::PlatformModule>();
    }

    /// <summary>
    /// モジュール初期化
    /// </summary>
    /// <returns></returns>
    const Bool LevelModule::_Start()
    {
        // レベル関連の準備
        {
            this->_pLevelManager = std::shared_ptr<Manager>(new Manager());
            this->_pLevelManager->Init();
        }

        return TRUE;
    }

    const Bool LevelModule::_Update(const Float32 in_fDeltaTime)
    {
        auto pPlatformModule = this->GetDependenceModule<Platform::PlatformModule>();

        // マウスやキーボードなどの各入力を渡す
        this->_pLevelManager->Update(in_fDeltaTime, pPlatformModule->Input());

        return TRUE;
    }
    const Bool LevelModule::_Release()
    {
        this->_pLevelManager->End();
        this->_pLevelManager.reset();

        return TRUE;
    }
}  // namespace Level
