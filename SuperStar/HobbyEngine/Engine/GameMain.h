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
    virtual Bool VStart(const Bool in_bDebug) = 0;

    /// <summary>
    /// ゲーム更新
    /// エンジン側が呼ぶ
    /// </summary>
    virtual Bool VUpdate(const Float32 in_fDeltaTime) = 0;

    /// <summary>
    /// ゲーム終了
    /// エンジン側が呼ぶ
    /// </summary>
    virtual Bool VEnd() = 0;
};
