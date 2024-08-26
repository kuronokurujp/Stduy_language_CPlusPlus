#pragma once

#include "Engine/Common/CustomVector.h"
#include "Engine/Math/Vector2.h"
#include "Engine/MiniEngine.h"

// 外部モジュール
#include "LevelModule.h"

namespace Level
{
    class LevelInGame_BG : public Level::Node
    {
        HE_CLASS_COPY_NG(LevelInGame_BG);
        HE_CLASS_MOVE_NG(LevelInGame_BG);

    public:
        LevelInGame_BG() : Level::Node() {}

        const Bool Begin() override final;
        const Bool End() override final;

        /// <summary>
        /// 更新
        /// </summary>
        void Update(const Float32 in_fDt, const Core::TaskData&) override final;

    private:
        Core::Common::CustomArray<Core::Math::Vector2, 64> _aPointPos;
        Core::Common::Handle _viewHandle;
    };
}  // namespace Level
