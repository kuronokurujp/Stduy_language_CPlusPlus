﻿#include "AssetManagerModule.h"

// 依存モジュール
#include "AssetDataBase.h"
#include "HobbyEngine/Platform/PlatformModule.h"

MODULE_GENRATE_DEFINITION(AssetManager::AssetManagerModule, AssetManager);

namespace AssetManager
{
    const Bool AssetManagerModule::Start()
    {
        // 利用するアセット数を設定
        this->_Reserve(1024);

        return TRUE;
    }

    const Bool AssetManagerModule::Update(const Float32 in_deltaTime)
    {
        return TRUE;
    }

    void AssetManagerModule::Unload(const Core::Common::Handle& in_rHandle)
    {
        if (in_rHandle.Null()) return;

        if (this->Empty()) return;

        AssetDataBase* p = this->_Ref(in_rHandle);
        if (p == NULL) return;

        p->_Unload();
    }

    const Bool AssetManagerModule::_Load(AssetDataBase* out_pAssetData)
    {
        Platform::FileSystemInterface* pFileSystem = this->GetPlatformModule()->File();
        if (out_pAssetData->_Load(*pFileSystem) == FALSE)
        {
            return FALSE;
        }

        return TRUE;
    }

    const Bool AssetManagerModule::Release()
    {
        // アセットハンドルがあれば解放する
        {
            auto assetList = this->GetUserDataList();
            for (auto it = assetList.begin(); it != assetList.end(); ++it)
            {
                this->Unload(it->_handle);
            }
        }

        return TRUE;
    }

}  // namespace AssetManager
