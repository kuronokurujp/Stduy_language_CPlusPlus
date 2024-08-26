#pragma once

#include "Engine/Math/Rect2.h"
#include "Engine/Math/Vector2.h"
#include "Engine/MiniEngine.h"
#include "Render/Color.h"

namespace Render
{
    // コマンド一覧
    enum CmdType
    {
        // 画面を指定色でクリア
        CMD_TYPE_SCREEN_CLEAR = 0,
        // テキスト描画
        CMD_TYPE_2D_TEXT_DRAW,
        // 矩形描画
        CMD_TYPE_2D_RECT_DRAW,
        // TODO: 点描画
        CMD_TYPE_2D_POINT_DRAW,
    };

    /// <summary>
    /// 2D画面にテキスト表示
    /// </summary>
    struct Cmd2DTextDraw
    {
        Float32 fX = 0.0f;
        Float32 fY = 0.0f;
        Color color;
        Core::Math::Rect2::EAnchor anchor = Core::Math::Rect2::EAnchor_Left;

        Char szChars[128] = {0};
    };

    /// <summary>
    /// 2D画面に矩形を表示
    /// </summary>
    struct Cmd2DRectDraw
    {
        Float32 fLeftX = 0.0f;
        Float32 fLeftY = 0.0f;

        Float32 fRightX = 0.0f;
        Float32 fRightY = 0.0f;

        Color color;
    };

    /// <summary>
    /// 2D上で点を描画
    /// </summary>
    struct Cmd2DPointDraw
    {
        Float32 fX = 0.0f;
        Float32 fY = 0.0f;
        Color color;
    };

    /// <summary>
    /// 画面を指定色でクリア
    /// </summary>
    struct CmdScreenClaer
    {
        Color color;
    };

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
            CmdScreenClaer screenClear;
            Cmd2DTextDraw text2DDraw;
            Cmd2DRectDraw rect2DDraw;
            Cmd2DPointDraw point2DDraw;

        } data;
    };

    /// <summary>
    /// 2Dテキスト描画
    /// </summary>
    extern void CreateCmd2DTextDraw(const Core::Common::Handle& in_rViewHandle,
                                    const Core::Math::Vector2& in_rPos,
                                    const Core::Common::StringBase& in_str, const Color& in_rColor,
                                    const Core::Math::Rect2::EAnchor in_eAnchor);

    /// <summary>
    /// 2D矩形描画
    /// </summary>
    extern void CreateCmd2DRectDraw(const Core::Common::Handle& in_rViewHandle,
                                    const Core::Math::Rect2& in_rRect, const Color& in_rColor);

    /// <summary>
    /// 2D点描画
    /// </summary>
    extern void CreateCmd2DPointDraw(const Core::Common::Handle& in_rViewHandle,
                                     const Core::Math::Vector2& in_rPos, const Color& in_rColor);

}  // namespace Render
