#pragma once

#include "Engine/Common/CustomMap.h"
#include "Engine/File/Path.h"
#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"

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

        using LocateMap =
            Core::Common::CustomFixMap<Core::Common::FixString128, SystemAssetData::LocateData, 32>;

        virtual const Bool _Load(Platform::FileInterface& in_rFileSystem) override;

        /// <summary>
        /// ロケートマップデータを探して取得
        /// 常駐しているメンバー変数を参照するようにしている
        /// 呼び出し側が参照したメンバー変数の書き換えたをできないようにするためにconstにしている
        /// </summary>
        const LocateMap& FindLocate(const Char* in_szLocateName);

    private:
        /// <summary>
        /// 各言語毎のデータ
        /// </summary>
        Core::Common::CustomFixMap<Core::Common::FixString128, LocateMap, 32> _locateDataMap;
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
        Core::Common::CustomFixMap<Core::Common::FixString128, Core::Common::FixString1024, 128>
            _textBuffMap;
    };

    /// <summary>
    /// 多言語対応のモジュール
    /// </summary>
    class LocalizationModule final : public Module::ModuleBase
    {
        HE_MODULE_GENRATE_DECLARATION(LocalizationModule);

    public:
        LocalizationModule();

        // ローカライズ設定データをロード(テキストファイル版)
        // バイナリファイル版も用意する
        const Bool LoadSystemFile(const Core::Common::StringBase&);
        // ローカライズ設定データをアンロード
        const Bool UnloadSystemFile();

        // ローカライズデータをロード
        const Bool LoadTextAll(const Core::Common::StringBase&);
        // ローカライズデータをアンロード
        const Bool UnloadTextAll(const Core::Common::StringBase&);

        // ローカライズテキストを取得
        const Char* Text(const Core::Common::StringBase& in_szrLocateName,
                         const Core::Common::StringBase& in_szrGroupName,
                         const Core::Common::StringBase& in_szrKey);

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool _Start() override final;
        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        virtual const Bool _Release() override final;

        const Bool _Update(const Float32 in_fDeltaTime) override final;

    private:
        using LocateTextMap =
            Core::Common::CustomFixMap<Core::Common::FixString128, Core::Common::Handle, 64>;

        Core::Common::Handle _sysAssetDataHandle;
        Core::Common::CustomFixMap<Core::Common::FixString128, LocateTextMap, 16> _locateDataMap;
    };
}  // namespace Localization
