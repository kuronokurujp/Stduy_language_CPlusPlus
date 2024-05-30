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
            FPS(Platform::TimeSystemInterface* in_pTimeInterface);

            /// <summary>
            /// 指定時間待機(ミリ秒)
            /// </summary>
            /// <param name="in_fps"></param>
            /// <returns></returns>
            const Bool UpdateWait(const Uint32 in_waitMSec);

            /// <summary>
            /// 更新時間取得(秒)
            /// </summary>
            /// <returns></returns>
            const Float32 GetDeltaTimeSec() const;
            const Float32 GetDeltaTimeMSec() const;

        private:
            static const Uint32 _timeAvgMax = 10;
            Uint32 _previousTime[_timeAvgMax] = { 0 };
            Uint32 _frameRate;

            /// <summary>
            /// プラットフォーム固有の時間制御インターフェイス
            /// </summary>
            Platform::TimeSystemInterface* _pTimeInterface;
        };
    }
}
