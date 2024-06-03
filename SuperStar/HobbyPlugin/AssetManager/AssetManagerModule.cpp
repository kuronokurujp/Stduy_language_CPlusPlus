#include "AssetManagerModule.h"

// 依存モジュール
#include "HobbyEngine/Platform/PlatformModule.h"

#include "AssetDataBase.h"

namespace AssetManager
{
    const Bool AssetManagerModule::Init()
    {
        // 利用するアセット数を設定
        this->Reserve(1024);

        return TRUE;
    }

    const Bool AssetManagerModule::End()
    {
        return TRUE;
    }

    const Bool AssetManagerModule::Update(const Float32 in_deltaTime)
    {
        return TRUE;
    }

    const Bool AssetManagerModule::_Load(AssetDataBase* out_pAssetData)
    {
        Platform::FileSystemInterface* pFileSystem = Platform::PlatformModule::I().File();
        if (out_pAssetData->_Load(*pFileSystem) == FALSE)
        {
            return FALSE;
        }

        return TRUE;
    }
}

