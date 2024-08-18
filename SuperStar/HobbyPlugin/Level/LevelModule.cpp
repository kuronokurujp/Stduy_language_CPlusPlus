#include "LevelModule.h"

#include "Engine/Memory/Memory.h"
#include "Level/LevelManager.h"

// モジュール
#include "EnhancedInputModule.h"

namespace Level
{
    LevelModule::LevelModule() : ModuleBase(ModuleName())
    {
        // 依存しているモジュールを設定
        // モジュールの依存設定していないと依存したモジュールが使えない
        this->_AppendDependenceModule<Level::LevelModule>();
        this->_AppendDependenceModule<Actor::ActorModule>();
        this->_AppendDependenceModule<Platform::PlatformModule>();
        this->_AppendDependenceModule<EnhancedInput::EnhancedInputModule>();
    }

    /// <summary>
    /// モジュール初期化
    /// </summary>
    /// <returns></returns>
    const Bool LevelModule::_Start()
    {
        // レベル関連の準備
        {
            this->_pLevelManager = Core::Memory::MakeCustomSharedPtr<Manager>();
            this->_pLevelManager->Init();
        }

        return TRUE;
    }

    const Bool LevelModule::_Update(const Float32 in_fDeltaTime)
    {
        // インプット入力対象に入力結果を送信
        auto pEnhancedInputModule = this->GetDependenceModule<EnhancedInput::EnhancedInputModule>();

        // 入力結果を渡す
        this->_pLevelManager->ProcessInput(in_fDeltaTime, pEnhancedInputModule->GetInputMap());

        // マウスやキーボードなどの各入力を渡す
        this->_pLevelManager->Update(in_fDeltaTime);

        return TRUE;
    }

    const Bool LevelModule::_Release()
    {
        this->_pLevelManager->End();
        this->_pLevelManager.reset();

        return TRUE;
    }
}  // namespace Level
