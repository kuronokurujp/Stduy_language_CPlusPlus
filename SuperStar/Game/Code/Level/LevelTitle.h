#pragma once

#include "Engine/MiniEngine.h"

// 外部モジュール
#include "LevelModule.h"

namespace Level
{
    class LevelTitle : public Level::Node
    {
        HE_CLASS_COPY_CONSTRUCT_NG(LevelTitle);

    public:
        LevelTitle();
        ~LevelTitle();

        const Bool Begin() override final;
        const Bool End() override final;

    private:
        Core::Common::Handle _layoutAssetHandle;
        Core::Common::Handle _uIWidgetHandle;
    };
}  // namespace Level
