#pragma once

#include "HobbyEngine/Core/Common/FixMap.h"
#include "HobbyEngine/Core/File/Path.h"
#include "HobbyEngine/MiniEngine.h"
#include "HobbyEngine/Module/Module.h"

// 依存モジュール
#include "HobbyPlugin/AssetManager/AssetManagerModule.h"

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

            LocateData(const Core::File::Path& in_textFileJsonPath)
            {
                this->_textFilePath = in_textFileJsonPath;
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
        const SYSTEM_ASSET_LOCATE_MAP& FindLocate(const Char* in_pLocateName);

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
        const Core::Common::FixString1024& GetText(const Char* in_pKey);

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
        LocalizationModule(const Char* in_pName) : ModuleBase(in_pName) {}

        // ローカライズ設定データをロード(テキストファイル版)
        // バイナリファイル版も用意する
        const Bool LoadSystemFile(const Core::Common::FixString128& in_rFilePath);
        // ローカライズ設定データをアンロード
        const Bool UnloadSystemFile();

        // ローカライズデータをロード
        const Bool LoadTextAll(const Core::Common::FixString128&);
        // ローカライズデータをアンロード
        const Bool UnloadTextAll(const Core::Common::FixString128&);

        // ローカライズテキストを取得
        const Char* Text(const Core::Common::FixString128& in_rLocateName,
                         const Core::Common::FixString128& in_rGroupName,
                         const Core::Common::FixString128& in_rKey);

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

        const Bool Update(const Float32 in_deltaTime) final override;

    private:
#define LOCATE_TEXT_MAP Core::Common::FixMap<Core::Common::FixString128, Core::Common::Handle, 64>

        Core::Common::Handle _sysAssetDataHandle;
        Core::Common::FixMap<Core::Common::FixString128, LOCATE_TEXT_MAP, 16> _locateDataMap;
    };
}  // namespace Localization

MODULE_GENRATE_DECLARATION(Localization::LocalizationModule, Localization);
