#include "AssetManagerModule.h"

#include "AssetManager/AssetDataBase.h"
#include "Engine/Platform/PlatformModule.h"

namespace AssetManager
{
    AssetManagerModule::AssetManagerModule() : ModuleBase(ModuleName())
    {
        // 依存モジュール
        this->_AppendDependenceModule<Platform::PlatformModule>();
    }

    const Bool AssetManagerModule::_Start()
    {
        // 利用するアセット数を設定
        this->_Reserve(1024);

        return TRUE;
    }

    const Bool AssetManagerModule::_Update(const Float32 in_fDeltaTime)
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

    const Bool AssetManagerModule::_Load(AssetDataBase* out)
    {
        auto pPlatformModule = this->GetDependenceModule<Platform::PlatformModule>();
        Platform::FileInterface* pFileSystem = pPlatformModule->File();
        if (out->_Load(*pFileSystem) == FALSE)
        {
            return FALSE;
        }

        return TRUE;
    }

    const Bool AssetManagerModule::_Release()
    {
        // アセットハンドルがあれば解放する
        {
            auto assetList = this->GetUserDataList();
            for (auto it = assetList->begin(); it != assetList->end(); ++it)
            {
                this->Unload(it->first);
            }
        }

        return TRUE;
    }

}  // namespace AssetManager
