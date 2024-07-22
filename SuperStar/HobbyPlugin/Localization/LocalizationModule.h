﻿#pragma once

#include "Common/CustomMap.h"
#include "File/Path.h"
#include "MiniEngine.h"
#include "Module/Module.h"

// 依存モジュール
#include "AssetManagerModule.h"

namespace Localization
{
    /// <summary>
    /// ローカライズのシステムアセットデータ
    /// </summary>
    class SystemAssetData final : public AssetManager::AssetDataToml
    {
    public:
        /// <summary>
        /// 言語のデータ
        /// </summary>
        struct LocateData
        {
        public:
            LocateData() {}
            LocateData(const LocateData& in_data) { this->_textFilePath = in_data._textFilePath; }

            LocateData(const Core::File::Path& in_rTextFileJsonPath)
            {
                this->_textFilePath = in_rTextFileJsonPath;
            }

            Core::File::Path _textFilePath;
        };

#define SYSTEM_ASSET_LOCATE_MAP \
    Core::Common::FixMap<Core::Common::FixString128, SystemAssetData::LocateData, 32>

        virtual const Bool _Load(Platform::FileSystemInterface& in_rFileSystem) override;

        /// <summary>
        /// ロケートマップデータを探して取得
        /// 常駐しているメンバー変数を参照するようにしている
        /// 呼び出し側が参照したメンバー変数の書き換えたをできないようにするためにconstにしている
        /// </summary>
        /// <param name="in_pLocateName"></param>
        /// <returns></returns>
        const SYSTEM_ASSET_LOCATE_MAP& FindLocate(const Char* in_szLocateName);

    private:
        /// <summary>
        /// 各言語毎のデータ
        /// </summary>
        Core::Common::FixMap<Core::Common::FixString128, SYSTEM_ASSET_LOCATE_MAP, 32>
            _locateDataMap;
    };

    /// <summary>
    /// 言語のテキストなどのアセットデータ
    /// </summary>
    class LocateAssetData final : public AssetManager::AssetDataJson
    {
    public:
        /// <summary>
        /// テキスト取得
        /// </summary>
        /// <param name="in_pKey"></param>
        /// <returns></returns>
        const Core::Common::FixString1024& GetText(const Char* in_szKey);

    private:
        Core::Common::FixMap<Core::Common::FixString128, Core::Common::FixString1024, 128>
            _textBuffMap;
    };

    /// <summary>
    /// 多言語対応のモジュール
    /// </summary>
    class LocalizationModule final : public Module::ModuleBase
    {
    public:
        LocalizationModule(const Char* in_szName) : ModuleBase(in_szName) {}

        // ローカライズ設定データをロード(テキストファイル版)
        // バイナリファイル版も用意する
        const Bool LoadSystemFile(const Core::Common::FixStringBase&);
        // ローカライズ設定データをアンロード
        const Bool UnloadSystemFile();

        // ローカライズデータをロード
        const Bool LoadTextAll(const Core::Common::FixStringBase&);
        // ローカライズデータをアンロード
        const Bool UnloadTextAll(const Core::Common::FixStringBase&);

        // ローカライズテキストを取得
        const Char* Text(const Core::Common::FixStringBase& in_szrLocateName,
                         const Core::Common::FixStringBase& in_szrGroupName,
                         const Core::Common::FixStringBase& in_szrKey);

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool Start() final override;
        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        virtual const Bool Release() override final;

        const Bool Update(const Float32 in_fDeltaTime) final override;

    private:
#define LOCATE_TEXT_MAP Core::Common::FixMap<Core::Common::FixString128, Core::Common::Handle, 64>

        Core::Common::Handle _sysAssetDataHandle;
        Core::Common::FixMap<Core::Common::FixString128, LOCATE_TEXT_MAP, 16> _locateDataMap;
    };
}  // namespace Localization

MODULE_GENRATE_DECLARATION(Localization::LocalizationModule, Localization);
