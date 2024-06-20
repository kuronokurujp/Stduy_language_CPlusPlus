#include "FPS.h"

#include "Platform/PlatformModule.h"

namespace Core
{
    namespace Time
    {
        FPS::FPS(Platform::TimeSystemInterface* in_pTimeInterface)
        {
            this->_pTimeInterface = in_pTimeInterface;
        }

        const Bool FPS::UpdateWait(const Uint32 in_waitMSec)
        {
            // 前フレームからin_waitMSec経ったら処理に進める
            if (in_waitMSec <
                (this->_pTimeInterface->NowMSec() - this->_previousTime[FPS::_timeAvgMax - 1]))
                return FALSE;

            // 帰ってくる時間の単位はmsec
            Uint32 currentTime = this->_pTimeInterface->NowMSec();
            // 新しい時間と一番古い時間の差を取得
            Uint32 frameTime10 = currentTime - this->_previousTime[0];
            // 格納した時間をずらす
            for (Uint32 i = 0; i < FPS::_timeAvgMax - 1; ++i)
                this->_previousTime[i] = this->_previousTime[i + 1];

            // 一番最後に新しい時間を設定
            this->_previousTime[FPS::_timeAvgMax - 1] = currentTime;

            // 1000で1秒
            // 1000 / frame_time_10 / gc_time_avg_max なのだが、
            // 式を展開すると 1000 * gc_time_avg_max / frame_time_10 となる
            this->_frameRate = 1000 * FPS::_timeAvgMax / frameTime10;

            return TRUE;
        }

        const Float32 FPS::GetDeltaTimeMSec() const
        {
            const Float32 oldMSec = static_cast<Float32>(this->_previousTime[FPS::_timeAvgMax - 1]);
            if (oldMSec <= 0.0f) return 0.0f;

            const Float32 nowMSec = static_cast<Float32>(this->_pTimeInterface->NowMSec());
            float deltaTime       = nowMSec - oldMSec;

            return deltaTime;
        }

        const Float32 FPS::GetDeltaTimeSec() const
        {
            Float32 deletaTime = this->GetDeltaTimeMSec();
            if (deletaTime <= 0.0f) return 0.0f;

            // 秒に変換
            deletaTime /= 1000.0f;
            // 定数以上値になったら補正する
            if (deletaTime > 0.05f) deletaTime = 0.05f;

            return deletaTime;
        }
    }  // namespace Time
}  // namespace Core
