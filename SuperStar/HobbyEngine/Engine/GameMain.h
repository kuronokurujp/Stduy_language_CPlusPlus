#pragma once

#include "Engine/MiniEngine.h"

/// <summary>
/// ゲームエントリポイント
/// アプリ側が継承しないといけない
/// </summary>
class EntryGameMain
{
public:
    /// <summary>
    /// ゲーム開始
    /// エンジン側が呼ぶ
    /// </summary>
    /// <param name="in_bDebug"></param>
    /// <returns></returns>
    virtual const Bool Start(const Bool in_bDebug) = 0;

    /// <summary>
    /// ゲーム更新
    /// エンジン側が呼ぶ
    /// </summary>
    /// <param name="in_deltaTime"></param>
    /// <returns></returns>
    virtual const Bool Update(const Float32 in_fDeltaTime) = 0;

    /// <summary>
    /// ゲーム終了
    /// エンジン側が呼ぶ
    /// </summary>
    /// <returns></returns>
    virtual const Bool End() = 0;
};
