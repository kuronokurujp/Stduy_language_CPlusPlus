#pragma once

#include "HobbyEngine/MiniEngine.h"
#include "HobbyEngine/Core/Common/Handle.h"
#include "HobbyEngine/Core/Common/PoolManager.h"
#include "HobbyEngine/Core/File/Path.h"

#include "HobbyEngine/Module/Module.h"

namespace AssetManager
{
    // 前方宣言
    class AssetDataBase;

    /// <summary>
    /// エンジンのアセット対応のモジュール
    /// </summary>
    class AssetManagerModule final : public Module::ModuleBase<AssetManagerModule>, Core::Common::BasePoolManager<AssetDataBase>
    {
    public:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool Init() final override;

        /// <summary>
        /// モジュール終了
        /// </summary>
        /// <returns></returns>
        const Bool End() final override;

        const Bool Update(const Float32 in_deltaTime) final override;

        template<class T>
        const Core::Common::Handle Load(const Char* in_pName, Core::File::Path in_rFilePath)
        {
            // インスタンスを確保
            auto p = this->_Alloc<T>();

            AssetDataBase* pAsset = p._pItem;
            pAsset->_Init(in_pName, in_rFilePath);

            // アセットのロードをして失敗したら確保したインスタンスを解放
            if (this->_Load(pAsset))
            {
                return p._handle;
            }

            this->_Free(p._handle, TRUE);
            return Core::Common::Handle();
        }

    private:
        const Bool _Load(AssetDataBase* out_pAssetData);
    };
}
