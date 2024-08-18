#pragma once

#include "Engine/MiniEngine.h"

// 外部モジュール
#include "LevelModule.h"

namespace Level
{
    class LevelInGame : public Level::Node
    {
        HE_CLASS_COPY_NG(LevelInGame);
        HE_CLASS_MOVE_NG(LevelInGame);

    public:
        LevelInGame() : Level::Node() {}

        const Bool Begin() override final;
        const Bool End() override final;

        /// <summary>
        /// 更新
        /// </summary>
        void Update(const Float32 in_fDt, const Core::TaskData&) override final;

    private:
        Core::Common::Handle _systemComponentHandle;
    };
}  // namespace Level
