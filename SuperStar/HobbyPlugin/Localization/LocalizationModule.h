#pragma once

#include "HobbyEngine/MiniEngine.h"
#include "HobbyEngine/Core/Common/FixMap.h"
#include "HobbyEngine/Core/File/Path.h"
#include "HobbyEngine/Module/Module.h"

// 依存モジュール
#include "HobbyPlugin/AssetManager/AssetManagerModule.h"
#include "HobbyPlugin/AssetManager/AssetDataBase.h"

namespace Localization
{
    /// <summary>
    /// ローカライズのシステムアセットデータ
    /// </summary>
    class SystemAssetData final : public AssetManager::AssetDataToml
    {
    };

    /// <summary>
    /// 言語のテキストなどのアセットデータ
    /// </summary>
    class LocateAssetData final : public AssetManager::AssetDataJson
    {
    };

    /// <summary>
    /// 多言語対応のモジュール
    /// </summary>
    class LocalizationModule final : public Module::ModuleBase<LocalizationModule>
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

        // TODO: ローカライズ設定データをロード(テキストファイル版)
        // バイナリファイル版も用意する
        const Bool LoadSystemFile(const Core::Common::FixString128& in_rFilePath);
        // TODO: ローカライズ設定データをアンロード
        const Bool UnloadSystemFile();

        // TODO: ローカライズデータをロード
        const Bool LoadTextAll(const Core::Common::FixString128&);
        // TODO: ローカライズデータをアンロード
        const Bool UnloadTextAll(const Core::Common::FixString128&);

        // TODO: ローカライズテキストを取得
        Core::Common::FixString1024 Text(
            const Core::Common::FixString128& in_rLocateName,
            const Core::Common::FixString128& in_rGropuName,
            const Core::Common::FixString128& in_rKey);
        
        // TODO: ローカライズテキストを置換フォーマット指定で取得

    private:
        /// <summary>
        /// 言語のデータ
        /// </summary>
        struct LocateData
        {
        public:
            LocateData() {}

            LocateData(const Core::File::Path& in_textFileJsonPath)
            {
                this->_textFilePath = in_textFileJsonPath;
            }

            Core::File::Path _textFilePath;
        };

        /// <summary>
        /// 各言語毎のデータ
        /// </summary>
        Core::Common::FixMap<
            Core::Common::FixString128, Core::Common::FixMap<
                Core::Common::FixString128, LocateData, 32>, 32> _locateDataMap;
    };
}
