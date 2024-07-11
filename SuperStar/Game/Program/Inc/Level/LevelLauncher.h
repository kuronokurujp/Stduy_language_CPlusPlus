#pragma once

#include "MiniEngine.h"

// 外部モジュール
#include "HobbyPlugin/Level/LevelManager.h"

namespace Level
{
    /// <summary>
    /// デバッグメインレベル
    /// </summary>
    class LevelLauncher : public Level::Node
    {
        E_CLASS_COPY_CONSTRUCT_NG(LevelLauncher);

    public:
        LevelLauncher() : Level::Node() {}
        const Bool Begin() override final;
        const Bool End() override final;

    private:
        Core::Common::Handle _hLayoutAsset;
    };
}  // namespace Level
