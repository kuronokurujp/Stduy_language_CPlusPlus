#include "DXLibModule.h"

#include "DxLib.h"

// 依存モジュール一覧
#include "HobbyPlugin/Render/RenderModule.h"

MODULE_GENRATE_DEFINITION(DXLib::DxLibModule, Platform);

namespace DXLib
{
    const Bool DxLibModule::Start()
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

    const Bool DxLibModule::Release()
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

    const Bool DxLibModule::BeforUpdate(const Float32 in_fDeltaTime)
    {
        Bool bRet = TRUE;
        {
            // 入力の前更新
            this->_input.BeforeUpdate(in_fDeltaTime);

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

    const Bool DxLibModule::Update(const Float32 in_fDeltaTime)
    {
        this->_input.Update(in_fDeltaTime);

        return TRUE;
    }

    const Bool DxLibModule::AfterUpdate(const Float32 in_fDeltaTime)
    {
        // 画面の反映
        ScreenFlip();

        this->_input.AfterUpdate(in_fDeltaTime);

        return TRUE;
    }

    void DxLibModule::BeginRender(void* in_pCmdBuff)
    {
        Render::CommandBuffer* pCmdBuff = reinterpret_cast<Render::CommandBuffer*>(in_pCmdBuff);
        HE_ASSERT(pCmdBuff);
    }

    void DxLibModule::Redner(void* in_pCmdBuff)
    {
        Render::CommandBuffer* pCmdBuff = reinterpret_cast<Render::CommandBuffer*>(in_pCmdBuff);
        HE_ASSERT(pCmdBuff);

        for (Uint32 i = 0; i < pCmdBuff->Size(); ++i)
        {
            Render::Command* pCmd = pCmdBuff->GetPtr(i);

            // TODO: コマンドに応じた描画処理をする
            switch (pCmd->uType)
            {
                // TODO: 矩形を描画
                case Render::CMD_TYPE_2D_RECT:
                {
                    Render::ComRect2D* pRect2D = &pCmd->data.rect2D;

                    Uint32 uColor =
                        GetColor(pRect2D->color.c32.r, pRect2D->color.c32.g, pRect2D->color.c32.b);
                    DrawBox(static_cast<int>(pRect2D->fLeftX), static_cast<int>(pRect2D->fLeftY),
                            static_cast<int>(pRect2D->fRightX), static_cast<int>(pRect2D->fRightY),
                            uColor, TRUE);
                    break;
                }
                // TODO: 2Dテキストを描画
                case Render::CMD_TYPE_2D_TEXT:
                {
                    Render::ComText2D* pText2D = &pCmd->data.text2D;

                    Uint32 uColor =
                        GetColor(pText2D->color.c32.r, pText2D->color.c32.g, pText2D->color.c32.b);
                    Core::Common::FixString1024 str(pText2D->szChars);

                    DrawString(static_cast<int>(pText2D->fX), static_cast<int>(pText2D->fY),
                               pText2D->szChars, uColor);
                    break;
                }
                default:
                    HE_ASSERT(0 && "存在しないコマンド");
            }
        }
    }

    void DxLibModule::EndRender(void* in_pCmdBuff)
    {
        Render::CommandBuffer* pCmdBuff = reinterpret_cast<Render::CommandBuffer*>(in_pCmdBuff);
        HE_ASSERT(pCmdBuff);
    }

}  // namespace DXLib
