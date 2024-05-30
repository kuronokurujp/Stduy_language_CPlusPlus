#pragma once

#include "MiniEngine.h"

#include "Module/Module.h"

namespace Localization
{
    /// <summary>
    /// レベル用の追加モジュール
    /// </summary>
    class LocalizationModule final : public Module::ModuleBase<LocalizationModule>
    {
    public:
        /// <summary>
        /// TODO: モジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool Init() final override;

        /// <summary>
        /// TODO: モジュール終了
        /// </summary>
        /// <returns></returns>
        const Bool End() final override;

        const Bool Update(const Float32 in_deltaTime) final override;

        // TODO: ローカライズ設定データをロード(テキストファイル版)
        // バイナリファイル版も用意する
        const Bool LoadSystemFile(const Core::Common::FixString128& in_rFilePath);
        // TODO: ローカライズ設定データをアンロード
        const Bool UnloadFile();

        // TODO: ローカライズデータをロード
        const Bool LoadTextGroup(const Core::Common::FixString128& in_rGroupName);
        // TODO: ローカライズデータをアンロード
        const Bool UnloadTextGroup(const Core::Common::FixString128& in_rGroupName);

        // TODO: ローカライズテキストを取得
        Core::Common::FixString1024 Text(const Core::Common::FixString128& in_rGropuName, const Core::Common::FixString128 in_key);
        
        // TODO: ローカライズテキストを置換フォーマット指定で取得
    };
}
