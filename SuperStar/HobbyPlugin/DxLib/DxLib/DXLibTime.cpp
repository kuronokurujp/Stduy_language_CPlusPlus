﻿#include "DXLibTime.h"

#include "DxLib.h"

namespace DXLib
{
    /// <summary>
    /// 現在の経過時間を取得(ミリセコンド)
    /// </summary>
    /// <returns></returns>
    const Uint32 TimeSystem::NowMSec()
    {
        return static_cast<Uint32>(GetNowCount());
    }

    /// <summary>
    /// 指定した時間停止(ミリセコンド)
    /// </summary>
    /// <param name="in_ms"></param>
    void TimeSystem::SleepMSec(const Uint32 in_uMs)
    {
        DxLib::SleepThread(static_cast<int>(in_uMs));
    }
}  // namespace DXLib