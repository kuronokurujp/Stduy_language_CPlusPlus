#pragma once

#include "Engine/MiniEngine.h"

// 外部モジュール
#include "LevelModule.h"

namespace Level
{
    class LevelInGame final : public Level::Node
    {
        HE_CLASS_COPY_NG(LevelInGame);
        HE_CLASS_MOVE_NG(LevelInGame);

    public:
        LevelInGame();

        const Bool VBegin() override final;
        const Bool VEnd() override final;

        /// <summary>
        /// 更新
        /// </summary>
        void VUpdate(const Float32 in_fDt, const Core::TaskData&) override final;

    private:
        Core::Common::Handle _systemComponentHandle;
        Core::Common::Handle _shotManagerComponentHandle;
        Core::Common::Handle _stageManagerComponentHandle;
    };
}  // namespace Level
