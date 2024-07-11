#pragma once

#include "../Color.h"
#include "Core/Common/FixString.h"
#include "Core/Core.h"
#include "Core/Math/Rect2.h"
#include "Core/Math/Vector2.h"

namespace Render
{
    // コマンド一覧
    enum CmdType
    {
        CMD_TYPE_2D_TEXT = 0,
        CMD_TYPE_2D_RECT,
    };

    /// <summary>
    /// 2D画面にテキスト表示
    /// </summary>
    struct ComText2D
    {
        Float32 x;
        Float32 y;

        Color color;

        Char chars[128];
    };

    extern void Cmd2DText(const Core::Math::Vector2& in_pos,
                          const Core::Common::FixStringBase& in_str, const Color in_color);

    /// <summary>
    /// 2D画面に矩形を表示
    /// </summary>
    struct ComRect2D
    {
        Float32 leftX;
        Float32 leftY;

        Float32 rightX;
        Float32 rightY;

        Color color;
    };

    extern void Cmd2DRect(const Core::Math::Rect2& in_rect, const Color in_color);

    /// <summary>
    /// 描画コマンド
    /// 各コマンドはこれをキャストしてデータ設定をする
    /// </summary>
    struct Command
    {
        Uint32 type = 0;
        union
        {
            // バッファ
            Uint64 work[128] = {};

            ComText2D text2D;
            ComRect2D rect2D;

        } data;
    };

    template <typename T>
    T* Cast(Command* in_pCom)
    {
        // TODO: キャストしたデータサイズがCommand型のを超えていないか
        return reinterpret_cast<T>(in_pCom);
    }
}  // namespace Render
