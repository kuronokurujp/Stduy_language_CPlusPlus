#include "Command.h"

#include "RenderModule.h"

namespace Render
{
    void Cmd2DText(const Core::Math::Vector2& in_rPos, const Core::Common::StringBase& in_szrName,
                   const Color& in_rColor)
    {
        // TODO: 必要なコマンド情報を作る
        Command cmd;
        {
            cmd.uType = CMD_TYPE_2D_TEXT;

            ComText2D* pText2D = &cmd.data.text2D;
            pText2D->fX        = in_rPos._fX;
            pText2D->fY        = in_rPos._fY;
            pText2D->color     = in_rColor;
            HE_STR_ERRNO e     = HE_STR_CPY_S(pText2D->szChars, HE_ARRAY_NUM(pText2D->szChars),
                                              in_szrName.Str(), in_szrName.Capacity());
            HE_ASSERT(HE_STR_SUCCESS(e) && "文字列コピーに失敗");
        }

        ModuleRender()->AddCmd(std::move(cmd));
    }

    void Cmd2DRect(const Core::Math::Rect2& in_rRect, const Color& in_rColor)
    {
        // 必要なコマンド情報を作る
        Command cmd;
        {
            cmd.uType          = CMD_TYPE_2D_RECT;
            ComRect2D* pRect2D = &cmd.data.rect2D;
            pRect2D->fLeftX    = in_rRect._fLeft;
            pRect2D->fLeftY    = in_rRect._fTop;
            pRect2D->fRightX   = in_rRect._fRight;
            pRect2D->fRightY   = in_rRect._fBottom;
            pRect2D->color     = in_rColor;
        }

        ModuleRender()->AddCmd(std::move(cmd));
    }
}  // namespace Render
