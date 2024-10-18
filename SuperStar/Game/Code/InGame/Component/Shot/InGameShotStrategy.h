#pragma once

#include "Engine/Math/Vector2.h"
#include "InGameShotStrategyInterface.h"
// エンジン最小インクルード
#include "Engine/MiniEngine.h"

namespace InGame
{
    /// <summary>
    /// 通常弾を打つ戦略アルゴリズム
    /// </summary>
    class InGameShotStrategyNormalBullet final : public InGameShotStrategyInterface
    {
    public:
        // 弾を打つ設定
        void VConfigure(ShotConfig* out) override final;

        // 弾を打つ処理
        void VShot(const ShotConfig& in_rConfig) override final;
    };

}  // namespace InGame
