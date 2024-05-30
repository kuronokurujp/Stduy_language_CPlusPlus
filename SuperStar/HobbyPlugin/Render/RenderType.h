#pragma once

#include "Core/Core.h"

namespace Render
{
    struct Color32
    {
        Uint8 r, g, b, a;
    };

    /// <summary>
    /// 色情報
    /// </summary>
    union Color
    {
        Uint32 c;
        Color32 c32;
    };

    // 色のデータ一式
    static const Color WhiteColor { 0xffffffff };
    static const Color BlackColor{ 0x000000ff };
}
