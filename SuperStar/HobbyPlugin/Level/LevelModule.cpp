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
        this->_AppendDependenceModule<Actor::ActorModule>();
        this->_AppendDependenceModule<Platform::PlatformModule>();
        this->_AppendDependenceModule<EnhancedInput::EnhancedInputModule>();
    }

    /// <summary>
    /// モジュール初期化
    /// </summary>
    const Bool LevelModule::_VStart()
    {
        // レベル関連の準備
        {
            this->_pLevelManager = Core::Memory::MakeCustomSharedPtr<Manager>();
            this->_pLevelManager->Init();
        }

        return TRUE;
    }

    const Bool LevelModule::_VRelease()
    {
        this->_pLevelManager->Release();
        this->_pLevelManager.reset();

        return TRUE;
    }

    const Bool LevelModule::_VBeforeUpdate(const Float32 in_fDt)
    {
        // 前更新でメインレベルの切り替えなどしている
        this->_pLevelManager->BeforeUpdate(in_fDt);

        return TRUE;
    }

    const Bool LevelModule::_VUpdate(const Float32 in_fDt)
    {
        // インプット入力対象に入力結果を送信
        auto pEnhancedInputModule = this->GetDependenceModule<EnhancedInput::EnhancedInputModule>();

        // 入力結果を渡す
        this->_pLevelManager->ProcessInput(in_fDt, pEnhancedInputModule->GetInputMap());

        // マウスやキーボードなどの各入力を渡す
        this->_pLevelManager->Update(in_fDt);

        return TRUE;
    }
    const Bool LevelModule::_VLateUpdate(const Float32 in_fDt)
    {
        this->_pLevelManager->LateUpdate(in_fDt);
        return TRUE;
    }
}  // namespace Level
