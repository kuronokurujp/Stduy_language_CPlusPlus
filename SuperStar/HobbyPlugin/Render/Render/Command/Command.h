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
        Core::Math::Rect2::EAnchor anchor = Core::Math::Rect2::EAnchor_Left;

        Char szChars[128] = {0};
    };

    extern void Cmd2DText(const Core::Math::Vector2& in_rPos,
                          const Core::Common::StringBase& in_str, const Color& in_rColor,
                          const Core::Math::Rect2::EAnchor in_eAnchor);

    /// <summary>
    /// 2D画面に矩形を表示
    /// </summary>
    struct ComRect2D
    {
        Float32 fLeftX = 0.0f;
        Float32 fLeftY = 0.0f;

        Float32 fRightX = 0.0f;
        Float32 fRightY = 0.0f;

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

            // TODO: バッファ超えていた場合はコンパイルエラーを出すことはできるのだろうか？
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
