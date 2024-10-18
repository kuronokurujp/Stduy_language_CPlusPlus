#pragma once

#include "Engine/Math/Vector2.h"
// エンジン最小インクルード
#include "Engine/MiniEngine.h"

namespace InGame
{
    /// <summary>
    /// 弾を打つアリゴリズム戦略
    /// </summary>
    class InGameShotStrategyInterface
    {
    public:
        struct ShotConfig
        {
            Core::Math::Vector2 pos;
            Core::Math::Vector2 dir;
            Uint32 uCollisionHashCoee = 0;
        };

    public:
        virtual void VConfigure(ShotConfig* out)         = 0;
        virtual void VShot(const ShotConfig& in_rConfig) = 0;
    };

}  // namespace InGame
