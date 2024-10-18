#pragma once

#include "Engine/Common/Handle.h"
#include "Engine/Common/PoolManager.h"
#include "Engine/File/Path.h"
#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "AssetManager/AssetDataBase.h"

namespace AssetManager
{
    // 前方宣言
    class AssetDataBase;

    /// <summary>
    /// エンジンのアセット対応のモジュール
    /// </summary>
    class AssetManagerModule final : public Module::ModuleBase,
                                     Core::Common::RuntimePoolManager<AssetDataBase>
    {
        HE_MODULE_GENRATE_DECLARATION(AssetManagerModule);

    public:
        AssetManagerModule();

        template <class T>
        typename std::enable_if<std::is_base_of<AssetDataBase, T>::value,
                                const Core::Common::Handle>::type
        Load(const Char* in_pName, const Core::File::Path& in_rFilePath)
        {
            HE_ASSERT(in_rFilePath.Empty() == FALSE);
            HE_ASSERT(in_pName);
            // TODO: 名前が重複した時はどうするかは考える

            // インスタンスを確保
            auto p = this->_Alloc<T>();

            AssetDataBase* pAsset = p._pItem;
            pAsset->_VInit(in_pName,
                           Core::File::Path(this->_mountDirPath.Str(), in_rFilePath.Str()));

            // アセットのロードをして失敗したら確保したインスタンスを解放
            if (this->_Load(pAsset))
            {
                return p._handle;
            }

            this->_Free(p._handle, TRUE);
            p._handle.Clear();

            return Core::Common::Handle();
        }

        void Unload(const Core::Common::Handle&);

        template <class T>
        typename std::enable_if<std::is_base_of<AssetDataBase, T>::value, T&>::type GetAsset(
            const Core::Common::Handle& in_rHandle)
        {
            T* p = reinterpret_cast<T*>(this->_Ref(in_rHandle));
            HE_ASSERT(p && "ロードしたアセットデータがない");
            return *p;
        }

        /// <summary>
        /// アセット格納ディレクトリ設定
        /// </summary>
        /// <param name="in_szMountDir"></param>
        void SetMountDir(const Char* in_szMountDir) { this->_mountDirPath = in_szMountDir; }

    protected:
        /// <summary>
        /// モジュール開始
        /// </summary>
        /// <returns></returns>
        Bool _VStart() override final;

        /// <summary>
        /// モジュール解放
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        Bool _VRelease() override final;

        Bool _VUpdate(const Float32 in_fDeltaTime) override final;

    private:
        Bool _Load(AssetDataBase* out_pAssetData);

    private:
        Core::File::Path _mountDirPath;
    };
}  // namespace AssetManager
