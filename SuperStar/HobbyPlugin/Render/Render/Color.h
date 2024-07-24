#pragma once

#include "Engine/Core.h"

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
    namespace RGB
    {
        static const Color White{0xffffffff};
        static const Color Black{0x000000ff};
        static const Color Red{0xff0000ff};
        static const Color Blue{0x0000ffff};
        static const Color Green{0x00ff00ff};
        static const Color Yellow{0xffff00ff};
        static const Color Cyan{0x00ffffff};
        static const Color Magenta{0xff00ffff};
        static const Color Gray{0x808080ff};
        static const Color Orange{0xffa5ff};
    };  // namespace RGB

}  // namespace Render
