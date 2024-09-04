#pragma once

#include "Engine/MiniEngine.h"

// 外部モジュール
#include "LevelModule.h"

namespace Level
{
    class LevelTitle final : public Level::Node
    {
        HE_CLASS_COPY_NG(LevelTitle);
        HE_CLASS_MOVE_NG(LevelTitle);

    public:
        LevelTitle() : Level::Node() {}

        const Bool VBegin() override final;
        const Bool VEnd() override final;

    protected:
        void _VProcessInput(const Float32 in_fDt,
                            const EnhancedInput::InputMap* in_pInputMap) override final;

    private:
        Core::Common::Handle _layoutAssetHandle;
        Core::Common::Handle _uIWidgetHandle;
        Core::Common::Handle _viewHandle;
    };
}  // namespace Level
