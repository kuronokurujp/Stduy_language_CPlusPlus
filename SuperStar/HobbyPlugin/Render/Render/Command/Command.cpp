#include "Command.h"

#include "RenderModule.h"

namespace Render
{
    void Command2DTextDraw(const Core::Common::Handle& in_rViewHandle,
                           const Core::Math::Vector2& in_rPos,
                           const Core::Common::StringBase& in_szrName, const Color& in_rColor,
                           const Core::Math::Rect2::EAnchor in_eAnchor)
    {
        HE_ASSERT(in_rViewHandle.Null() == FALSE);

        // 必要なコマンド情報を作る
        Command cmd;
        {
            cmd.uType = ECmdType_2DTextDraw;

            Cmd2DTextDraw* pText2D = &cmd.data.text2DDraw;
            static_assert(sizeof(cmd.data.text2DDraw) <= sizeof(cmd.data.ulaWork));

            pText2D->fX     = in_rPos._fX;
            pText2D->fY     = in_rPos._fY;
            pText2D->color  = in_rColor;
            pText2D->anchor = in_eAnchor;
            HE_STR_ERRNO e  = HE_STR_CPY_S(pText2D->szChars, HE_ARRAY_NUM(pText2D->szChars),
                                           in_szrName.Str(), in_szrName.Capacity());
            HE_ASSERT(HE_STR_SUCCESS(e) && "文字列コピーに失敗");
        }

        auto pModule = Module::ModuleManager::I().Get<RenderModule>();
        auto pView   = pModule->GetView(in_rViewHandle);
        HE_ASSERT(pView);
        if (pView == NULL) return;

        pView->AddCmd(std::move(cmd));
    }

    void Command2DRectDraw(const Core::Common::Handle& in_rViewHandle,
                           const Core::Math::Rect2& in_rRect, const Color& in_rColor)
    {
        HE_ASSERT(in_rViewHandle.Null() == FALSE);

        // 必要なコマンド情報を作る
        Command cmd;
        {
            cmd.uType              = ECmdType_2DRectDraw;
            Cmd2DRectDraw* pRect2D = &cmd.data.rect2DDraw;
            static_assert(sizeof(cmd.data.rect2DDraw) <= sizeof(cmd.data.ulaWork));

            pRect2D->fLeftX  = in_rRect._fLeft;
            pRect2D->fLeftY  = in_rRect._fTop;
            pRect2D->fRightX = in_rRect._fRight;
            pRect2D->fRightY = in_rRect._fBottom;
            pRect2D->color   = in_rColor;
        }

        auto pModule = Module::ModuleManager::I().Get<RenderModule>();
        auto pView   = pModule->GetView(in_rViewHandle);
        HE_ASSERT(pView);
        if (pView == NULL) return;

        pView->AddCmd(std::move(cmd));
    }

    void Command2DPointDraw(const Core::Common::Handle& in_rViewHandle,
                            const Core::Math::Vector2& in_rPos, const Color& in_rColor)
    {
        HE_ASSERT(in_rViewHandle.Null() == FALSE);
        // 必要なコマンド情報を作る
        Command cmd;
        {
            cmd.uType                = ECmdType_2DPointDraw;
            Cmd2DPointDraw* pPoint2D = &cmd.data.point2DDraw;
            static_assert(sizeof(cmd.data.point2DDraw) <= sizeof(cmd.data.ulaWork));

            pPoint2D->fX    = in_rPos._fX;
            pPoint2D->fY    = in_rPos._fY;
            pPoint2D->color = in_rColor;
        }

        auto pModule = Module::ModuleManager::I().Get<RenderModule>();
        auto pView   = pModule->GetView(in_rViewHandle);
        HE_ASSERT(pView);
        if (pView == NULL) return;

        pView->AddCmd(std::move(cmd));
    }

    void CommandClsScreen(const Core::Common::Handle& in_rViewHandle, const Color& in_rColor)
    {
        HE_ASSERT(in_rViewHandle.Null() == FALSE);

        // 必要なコマンド情報を作る
        Command cmd;
        {
            cmd.uType                  = ECmdType_ClsScreen;
            CmdClsScreen* pClsScreen = &cmd.data.clsScree;
            static_assert(sizeof(cmd.data.clsScree) <= sizeof(cmd.data.ulaWork));
        }

        auto pModule = Module::ModuleManager::I().Get<RenderModule>();
        auto pView   = pModule->GetView(in_rViewHandle);
        HE_ASSERT(pView);
        if (pView == NULL) return;

        pView->AddCmd(std::move(cmd));
    }

}  // namespace Render
