#pragma once

#include "Core/Core.h"

// 前方宣言
namespace Platform
{
    class TimeSystemInterface;
}

/// <summary>
/// ゲーム中のFPS制御
/// </summary>
namespace Core
{
    namespace Time
    {
        class FPS
        {
        public:
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

        private:
            static const Uint32 _timeAvgMax   = 10;
            Uint32 _previousTime[_timeAvgMax] = {0};
            Uint32 _frameRate                 = 0;
        };
    }  // namespace Time
}  // namespace Core
