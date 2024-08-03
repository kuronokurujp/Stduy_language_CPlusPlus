#pragma once

#include "Engine/Core.h"

// 前方宣言
namespace Platform
{
    class TimeSystemInterface;
}

/// <summary>
/// ゲーム中のFPS制御
/// </summary>
namespace Core::Time
{
    class FPS
    {
    public:
        FPS() {}
        FPS(Platform::TimeSystemInterface* in_pTimeInterface);

        /// <summary>
        /// 指定時間待機(ミリ秒)
        /// </summary>
        /// <param name="in_fps"></param>
        /// <returns></returns>
        const Bool UpdateWait(Platform::TimeSystemInterface* in_pTimeInterface,
                              const Uint32 in_waitMSec);

        /// <summary>
        /// 更新時間取得(秒)
        /// </summary>
        /// <returns></returns>
        const Float32 GetDeltaTimeSec(Platform::TimeSystemInterface* in_pTimeInterface) const;
        const Float32 GetDeltaTimeMSec(Platform::TimeSystemInterface* in_pTimeInterface) const;

        inline const Uint32 GetFrameRate() const { return this->_uFrameRate; }

    private:
        static const Uint32 _uTimeAvgMax     = 5;
        Uint32 _uaPreviousTime[_uTimeAvgMax] = {0};
        Uint32 _uFrameRate                   = 0;
    };
}  // namespace Core::Time
