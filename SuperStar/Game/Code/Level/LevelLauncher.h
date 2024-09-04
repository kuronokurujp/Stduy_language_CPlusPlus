#pragma once

#include "Engine/MiniEngine.h"

// 外部モジュール
#include "LevelModule.h"

namespace Level
{
    /// <summary>
    /// デバッグメインレベル
    /// </summary>
    class LevelLauncher final : public Level::Node
    {
        HE_CLASS_COPY_NG(LevelLauncher);
        HE_CLASS_MOVE_NG(LevelLauncher);

    public:
        LevelLauncher() : Level::Node() {}
        const Bool VBegin() override final;
        const Bool VEnd() override final;

    private:
        Core::Common::Handle _layoutAssetHandle;
        Core::Common::Handle _uIWidgetHandle;
        Core::Common::Handle _viewHandle;
    };
}  // namespace Level
