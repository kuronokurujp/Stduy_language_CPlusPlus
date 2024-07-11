#include "LevelModule.h"

#include "Core/Memory/Memory.h"
#include "LevelManager.h"

MODULE_GENRATE_DEFINITION(Level::LevelModule, Level);

namespace Level
{
    /// <summary>
    /// モジュール初期化
    /// </summary>
    /// <returns></returns>
    const Bool LevelModule::Start()
    {
        // レベル関連の準備
        {
            this->_pLevelManager = std::shared_ptr<Manager>(new Manager());
            this->_pLevelManager->Init();
        }

        return TRUE;
    }

    const Bool LevelModule::Update(const Float32 in_deltaTime)
    {
        // マウスやキーボードなどの各入力を渡す
        this->_pLevelManager->Update(in_deltaTime, this->GetPlatformModule()->Input());

        return TRUE;
    }
    const Bool LevelModule::Release()
    {
        this->_pLevelManager->End();
        this->_pLevelManager.reset();

        return TRUE;
    }
}  // namespace Level
