#pragma once

#include "Engine/Core.h"

// プラットフォームのスクリーンインターフェイス
// スクリーン情報取得や設定などができる

namespace Platform
{
    class ScreenInterface
    {
    public:
        virtual const Uint32 VWidth() const  = 0;
        virtual const Uint32 VHeight() const = 0;
    };

}  // namespace Platform
