#include "DXLibModule.h"

#include "DxLib.h"

// 依存モジュール一覧
#include "HobbyPlugin/Render/RenderModule.h"

MODULE_GENRATE_DEFINITION(DXLib::DxLibModule, DxLib);

namespace DXLib
{
    const Bool DxLibModule::Init()
    {
        Bool bRet = TRUE;
        {
            // WindowModeにしている
            ChangeWindowMode(TRUE);

            // DxLib初期化
            if (DxLib_Init() == -1)
            {
                bRet = FALSE;
            }
            else
            {
                // デバッグ時のみログファイルを出す
#ifdef _DEBUG
                SetOutApplicationLogValidFlag(TRUE);
#else
                SetOutApplicationLogValidFlag(FALSE);
#endif
            }

            this->_input.Init();
        }

        return bRet;
    }

    const Bool DxLibModule::End()
    {
        // DxLibの後始末
        DxLib_End();

        return TRUE;
    }

    const Bool DxLibModule::CreateMainWindow()
    {
        return FALSE;
    }

    const Bool DxLibModule::ReleaseAllWindows()
    {
        return FALSE;
    }

    const Bool DxLibModule::BeforUpdate(const Float32 in_deltaTime)
    {
        Bool bRet = TRUE;
        {
            // 入力の前更新
            this->_input.BeforeUpdate(in_deltaTime);

            if ((ProcessMessage() != 0))
            {
                bRet = FALSE;
            }
            else
            {
                // 画面の描画クリア
                ClsDrawScreen();
            }
        }

        return bRet;
    }

    const Bool DxLibModule::Update(const Float32 in_deltaTime)
    {
        this->_input.Update(in_deltaTime);

        return TRUE;
    }

    const Bool DxLibModule::AfterUpdate(const Float32 in_deltaTime)
    {
        // 画面の反映
        ScreenFlip();

        this->_input.AfterUpdate(in_deltaTime);

        return TRUE;
    }

    void DxLibModule::BeginRender(void* in_pCmdBuff)
    {
        Render::CommandBuffer* pCmdBuff = reinterpret_cast<Render::CommandBuffer*>(in_pCmdBuff);
        E_ASSERT(pCmdBuff);
    }

    void DxLibModule::Redner(void* in_pCmdBuff)
    {
        Render::CommandBuffer* pCmdBuff = reinterpret_cast<Render::CommandBuffer*>(in_pCmdBuff);
        E_ASSERT(pCmdBuff);

        for (Uint32 i = 0; i < pCmdBuff->Size(); ++i)
        {
            Render::Command* cmd = pCmdBuff->GetPtr(i);

            // TODO: コマンドに応じた描画処理をする
            switch (cmd->type)
            {
                // TODO: 矩形を描画
                case Render::CMD_TYPE_2D_RECT:
                {
                    Render::ComRect2D* pRect2D = &cmd->data.rect2D;

                    Uint32 cr =
                        GetColor(pRect2D->color.c32.r, pRect2D->color.c32.g, pRect2D->color.c32.b);
                    DrawBox(static_cast<int>(pRect2D->leftX), static_cast<int>(pRect2D->leftY),
                            static_cast<int>(pRect2D->rightX), static_cast<int>(pRect2D->rightY),
                            cr, TRUE);
                    break;
                }
                // TODO: 2Dテキストを描画
                case Render::CMD_TYPE_2D_TEXT:
                {
                    Render::ComText2D* pText2D = &cmd->data.text2D;

                    Uint32 cr =
                        GetColor(pText2D->color.c32.r, pText2D->color.c32.g, pText2D->color.c32.b);
                    Core::Common::FixString1024 str(pText2D->chars);

                    DrawString(static_cast<int>(pText2D->x), static_cast<int>(pText2D->y),
                               pText2D->chars, cr);
                    break;
                }
                default:
                    E_ASSERT(0 && "存在しないコマンド");
            }
        }
    }

    void DxLibModule::EndRender(void* in_pCmdBuff)
    {
        Render::CommandBuffer* pCmdBuff = reinterpret_cast<Render::CommandBuffer*>(in_pCmdBuff);
        E_ASSERT(pCmdBuff);
    }
}  // namespace DXLib
