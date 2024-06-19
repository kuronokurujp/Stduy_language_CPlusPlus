#include "LevelModule.h"

#include "Core/Memory/Memory.h"
#include "LevelManager.h"

MODULE_GENRATE_DEFINITION(Level::LevelModule, Level);

namespace Level
{
    /// <summary>
    /// TODO: モジュール初期化
    /// </summary>
    /// <returns></returns>
    const Bool LevelModule::Init()
    {
        // レベル関連の準備
        {
            this->_pLevelManager = std::shared_ptr<Manager>(new Manager());
            this->_pLevelManager->Init();
        }

        return TRUE;
    }

    /// <summary>
    /// TODO: モジュール終了
    /// </summary>
    /// <returns></returns>
    const Bool LevelModule::End()
    {
        this->_pLevelManager->End();
        this->_pLevelManager.reset();

        return TRUE;
    }

    const Bool LevelModule::Update(const Float32 in_deltaTime)
    {
        // TODO: マウスやキーボードなどの各入力を渡す
        this->_pLevelManager->Update(in_deltaTime, Platform::PlatformModule::I().Input());

        return TRUE;
    }
}
