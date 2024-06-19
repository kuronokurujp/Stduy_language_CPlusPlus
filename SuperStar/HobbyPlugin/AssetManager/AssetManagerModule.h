#pragma once

#include "HobbyEngine/MiniEngine.h"
#include "HobbyEngine/Core/Common/Handle.h"
#include "HobbyEngine/Core/Common/PoolManager.h"
#include "HobbyEngine/Core/File/Path.h"

#include "HobbyEngine/Module/Module.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "AssetDataBase.h"

namespace AssetManager
{
    // 前方宣言
    class AssetDataBase;

    /// <summary>
    /// エンジンのアセット対応のモジュール
    /// </summary>
    class AssetManagerModule final :
        public Module::ModuleBase<AssetManagerModule>, Core::Common::BasePoolManager<AssetDataBase>
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
        typename std::enable_if<std::is_base_of<AssetDataBase, T>::value, const Core::Common::Handle>::type
        Load(const Char* in_pName, const Core::File::Path& in_rFilePath)
        {
            E_ASSERT(in_rFilePath.IsEmpty() == FALSE);
            E_ASSERT(in_pName);
            // TODO: 名前が重複した時はどうするかは考える

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

        void Unload(const Core::Common::Handle&);

        template<class T>
        typename std::enable_if<std::is_base_of<AssetDataBase, T>::value, T&>::type
        GetAsset(const Core::Common::Handle& in_rHandle)
        {
            T* p = reinterpret_cast<T*>(this->_Ref(in_rHandle));
            E_ASSERT(p && "ロードしたアセットデータがない");
            return *p;
        }

    private:
        const Bool _Load(AssetDataBase* out_pAssetData);
    };
}

MODULE_GENRATE_DECLARATION(AssetManager::AssetManagerModule, AssetManager);
