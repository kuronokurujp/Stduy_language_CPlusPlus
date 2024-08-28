#include "FPS.h"

#include "Engine/Platform/PlatformModule.h"

namespace Core::Time
{
    FPS::FPS(Platform::TimeInterface* in_pTimeInterface)
    {
        // 帰ってくる時間の単位はmsec
        Uint32 uCurrentTime = in_pTimeInterface->VNowMSec();
        // 格納した時間をずらす
        for (Uint32 i = 0; i < FPS::_uTimeAvgMax - 1; ++i) this->_uaPreviousTime[i] = uCurrentTime;
    }

    const Bool FPS::UpdateWait(Platform::TimeInterface* in_pTimeInterface,
                               const Uint32 in_uWaitMSec)
    {
        HE_ASSERT(in_pTimeInterface);

        // 前フレームからin_waitMSec経ったら処理に進める
        const Uint32 nowMSec = in_pTimeInterface->VNowMSec();
        Bool bWait =
            (in_uWaitMSec <= (nowMSec - this->_uaPreviousTime[FPS::_uTimeAvgMax - 1]) ? FALSE
                                                                                      : TRUE);

        // 待機時間より遅い
        if (bWait) return TRUE;

        // 格納した時間をずらす
        // バッファ移動に対応できなかったけ？
        Uint32 uSize = HE_ARRAY_SIZE(this->_uaPreviousTime) - sizeof(this->_uaPreviousTime[0]);
        ::memmove(this->_uaPreviousTime, &this->_uaPreviousTime[1], uSize);

        // フレームレートを計算
        {
            // 最新の時間と前フレーム前の時間との差
            Uint32 uFrameTime10 = nowMSec - this->_uaPreviousTime[0];
            // 0除算を回避
            if (0 < uFrameTime10)
            {
                // 1000で1秒
                // 1000 / frame_time_10 / gc_time_avg_max なのだが、
                // 式を展開すると 1000 * gc_time_avg_max / frame_time_10 となる
                this->_uFrameRate = (1000 * FPS::_uTimeAvgMax) / uFrameTime10;
            }
        }

        // 更新
        this->_uaPreviousTime[FPS::_uTimeAvgMax - 1] = nowMSec;

        return FALSE;
    }

    const Float32 FPS::GetDeltaTimeMSec(Platform::TimeInterface* in_pTimeInterface) const
    {
        HE_ASSERT(in_pTimeInterface);

        const Float32 fOldMSec = static_cast<Float32>(this->_uaPreviousTime[FPS::_uTimeAvgMax - 1]);
        if (fOldMSec <= 0.0f) return 0.0f;

        const Float32 fNowMSec = static_cast<Float32>(in_pTimeInterface->VNowMSec());
        Float32 fDeltaTime     = fNowMSec - fOldMSec;

        return fDeltaTime;
    }

    const Float32 FPS::GetDeltaTimeSec(Platform::TimeInterface* in_pTimeInterface) const
    {
        HE_ASSERT(in_pTimeInterface);

        Float32 fDeletaTime = this->GetDeltaTimeMSec(in_pTimeInterface);
        if (fDeletaTime <= 0.0f) return 0.0f;

        // 秒に変換
        fDeletaTime /= 1000.0f;
        // 定数以上値になったら補正する
        if (fDeletaTime > 0.05f) fDeletaTime = 0.05f;

        return fDeletaTime;
    }
}  // namespace Core::Time
