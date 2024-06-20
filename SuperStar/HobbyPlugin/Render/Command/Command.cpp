#include "Command.h"

#include "../RenderModule.h"

namespace Render
{
    void Cmd2DText(const Core::Math::Vector2& in_pos, const Core::Common::FixStringBase& in_str,
                   const Color in_color)
    {
        // TODO: 必要なコマンド情報を作る
        Command cmd;
        {
            cmd.type = CMD_TYPE_2D_TEXT;

            ComText2D* pText2D = &cmd.data.text2D;
            pText2D->x         = in_pos.x;
            pText2D->y         = in_pos.y;
            E_STR_CPY_S(pText2D->chars, E_ARRAY_SIZE(pText2D->chars), in_str.Str(), in_str.Size()),
                pText2D->color = in_color;
        }

        RenderModule::I().AddCmd(std::move(cmd));
    }

    void Cmd2DRect(const Core::Math::Rect2& in_rect, const Color in_color)
    {
        // TODO: 必要なコマンド情報を作る
        Command cmd;
        {
            cmd.type           = CMD_TYPE_2D_RECT;
            ComRect2D* pRect2D = &cmd.data.rect2D;
            pRect2D->leftX     = in_rect.left;
            pRect2D->leftY     = in_rect.top;
            pRect2D->rightX    = in_rect.right;
            pRect2D->rightY    = in_rect.bottom;
            pRect2D->color     = in_color;
        }

        RenderModule::I().AddCmd(std::move(cmd));
    }
}  // namespace Render
