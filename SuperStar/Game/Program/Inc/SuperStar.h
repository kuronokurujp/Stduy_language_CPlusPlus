#pragma once

#include "resource.h"

#include "GameMain.h"

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
    /// <param name="in_bDebug"></param>
    /// <returns></returns>
    const Bool Start(const Bool in_bDebug) override final;

    /// <summary>
    /// ゲーム更新
    /// エンジン側が呼ぶ
    /// </summary>
    /// <param name="in_deltaTime"></param>
    /// <returns></returns>
    const Bool Update(const Float32 in_deltaTime) override final;

    /// <summary>
    /// ゲーム終了
    /// エンジン側が呼ぶ
    /// </summary>
    /// <returns></returns>
    const Bool End() override final;

};
