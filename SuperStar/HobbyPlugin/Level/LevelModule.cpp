#include "LevelModule.h"

#include "Code/LevelManager.h"
#include "Memory/Memory.h"

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

    const Bool LevelModule::Update(const Float32 in_fDeltaTime)
    {
        // マウスやキーボードなどの各入力を渡す
        this->_pLevelManager->Update(in_fDeltaTime, this->GetPlatformModule()->Input());

        return TRUE;
    }
    const Bool LevelModule::Release()
    {
        this->_pLevelManager->End();
        this->_pLevelManager.reset();

        return TRUE;
    }
}  // namespace Level
