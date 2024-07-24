#pragma once

#include "Engine/Common/CustomString.h"
#include "Engine/Core.h"
#include "Engine/Math/Rect2.h"
#include "Engine/Math/Vector2.h"
#include "Render/Color.h"

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
        Float32 fX = 0.0f;
        Float32 fY = 0.0f;

        Color color;

        Char szChars[128] = {0};
    };

    extern void Cmd2DText(const Core::Math::Vector2& in_rPos,
                          const Core::Common::FixStringBase& in_str, const Color& in_rColor);

    /// <summary>
    /// 2D画面に矩形を表示
    /// </summary>
    struct ComRect2D
    {
        Float32 fLeftX;
        Float32 fLeftY;

        Float32 fRightX;
        Float32 fRightY;

        Color color;
    };

    extern void Cmd2DRect(const Core::Math::Rect2& in_rRect, const Color& in_rColor);

    /// <summary>
    /// 描画コマンド
    /// 各コマンドはこれをキャストしてデータ設定をする
    /// </summary>
    struct Command
    {
        Uint32 uType = 0;
        union
        {
            // バッファ
            Uint64 ulaWork[128] = {};

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
