#pragma once

// プラットフォームのスクリーンインターフェイス
// スクリーン情報取得や設定などができる

#include "Engine/Platform/PlatformScreen.h"

namespace DXLib
{
    class Screen final : public Platform::ScreenInterface
    {
    public:
        const Bool Init();

        const Uint32 VWidth() const override final;
        const Uint32 VHeight() const override final;

    private:
        Uint32 _w = 0;
        Uint32 _h = 0;
    };

}  // namespace DXLib
