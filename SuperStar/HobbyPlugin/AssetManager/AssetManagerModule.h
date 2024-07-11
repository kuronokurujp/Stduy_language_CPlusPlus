#pragma once

#include "HobbyEngine/Core/Common/Handle.h"
#include "HobbyEngine/Core/Common/PoolManager.h"
#include "HobbyEngine/Core/File/Path.h"
#include "HobbyEngine/MiniEngine.h"
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
    class AssetManagerModule final : public Module::ModuleBase,
                                     Core::Common::BasePoolManager<AssetDataBase>
    {
    public:
        AssetManagerModule(const Char* in_pName) : ModuleBase(in_pName) {}

        template <class T>
        typename std::enable_if<std::is_base_of<AssetDataBase, T>::value,
                                const Core::Common::Handle>::type
        Load(const Char* in_pName, const Core::File::Path& in_rFilePath)
        {
            E_ASSERT(in_rFilePath.Empty() == FALSE);
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

        template <class T>
        typename std::enable_if<std::is_base_of<AssetDataBase, T>::value, T&>::type GetAsset(
            const Core::Common::Handle& in_rHandle)
        {
            T* p = reinterpret_cast<T*>(this->_Ref(in_rHandle));
            E_ASSERT(p && "ロードしたアセットデータがない");
            return *p;
        }

    protected:
        /// <summary>
        /// モジュール開始
        /// </summary>
        /// <returns></returns>
        const Bool Start() final override;

        /// <summary>
        /// モジュール解放
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        virtual const Bool Release() override final;

        const Bool Update(const Float32 in_deltaTime) final override;

    private:
        const Bool _Load(AssetDataBase* out_pAssetData);
    };
}  // namespace AssetManager

MODULE_GENRATE_DECLARATION(AssetManager::AssetManagerModule, AssetManager);
