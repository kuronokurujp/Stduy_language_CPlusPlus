#pragma once

#include "Engine/GameMain.h"
#include "Resource.h"

/// <summary>
/// アプリのゲームメインのエントリー
/// ゲームの起動から更新,終了のロジックを記述
/// </summary>
class AppEntryGameMain final : public EntryGameMain
{
public:
    /// <summary>
    /// ゲーム開始
    /// エンジン側が呼ぶ
    /// </summary>
    Bool VStart(const Bool in_bDebug) override final;

    /// <summary>
    /// ゲーム更新
    /// エンジン側が呼ぶ
    /// </summary>
    Bool VUpdate(const Float32 in_deltaTime) override final;

    /// <summary>
    /// ゲーム終了
    /// エンジン側が呼ぶ
    /// </summary>
    Bool VEnd() override final;
};
