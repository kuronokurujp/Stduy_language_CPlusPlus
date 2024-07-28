#include "DXLibModule.h"

#include "DxLib.h"

// 依存モジュール一覧
#include "RenderModule.h"

MODULE_GENRATE_DEFINITION(DXLib::DXLibModule, Platform);

namespace DXLib
{
    const Bool DXLibModule::Start()
    {
        Bool bRet = TRUE;
        {
            // WindowModeにしている
            ChangeWindowMode(TRUE);
            // ウィンドウサイズを自由に変える事はしない
            SetWindowSizeChangeEnableFlag(FALSE);

            // TODO: サイズは適当
            {
                auto result = SetGraphMode(640, 480, 32);
                switch (result)
                {
                    case DX_CHANGESCREEN_OK:
                    {
                        HE_LOG_LINE(HE_STR_TEXT("画面モード変更成功"));
                        break;
                    }
                    case DX_CHANGESCREEN_RETURN:
                    {
                        HE_LOG_LINE(HE_STR_TEXT(" 画面の変更は失敗し元の画面モードに戻された"));
                        break;
                    }
                    case DX_CHANGESCREEN_DEFAULT:
                    {
                        HE_LOG_LINE(HE_STR_TEXT("画面の変更は失敗し標準の画面モードに変更された"));
                        break;
                    }
                }
            }

            // 描画先を裏画面にセット
            SetDrawScreen(DX_SCREEN_BACK);

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

    const Bool DXLibModule::Release()
    {
        // DxLibの後始末
        DxLib_End();

        return TRUE;
    }
    const Bool DXLibModule::CreateMainWindow()
    {
        return FALSE;
    }

    const Bool DXLibModule::ReleaseAllWindows()
    {
        return FALSE;
    }

    const Bool DXLibModule::BeforUpdate(const Float32 in_fDeltaTime)
    {
        Bool bRet = TRUE;
        {
            // 入力の前更新
            this->_input.BeforeUpdate(in_fDeltaTime);

            // 0以外だと画面終了メッセージとみなしている
            if ((ProcessMessage() != 0))
            {
                bRet = FALSE;
            }
        }

        return bRet;
    }

    const Bool DXLibModule::Update(const Float32 in_fDeltaTime)
    {
        this->_input.Update(in_fDeltaTime);

        return TRUE;
    }

    const Bool DXLibModule::AfterUpdate(const Float32 in_fDeltaTime)
    {
        this->_input.AfterUpdate(in_fDeltaTime);

        return TRUE;
    }

    void DXLibModule::BeginRender()
    {
        // 画面の描画クリア
        ClearDrawScreen();
    }

    void DXLibModule::Redner()
    {
        Render::RenderModule* pRenderModule = ModuleRender();
        HE_ASSERT(pRenderModule);

        Render::CommandBuffer* pCmdBuff =
            reinterpret_cast<Render::CommandBuffer*>(pRenderModule->GetCmdBuff());
        HE_ASSERT(pCmdBuff);

        for (Uint32 i = 0; i < pCmdBuff->Size(); ++i)
        {
            const Render::Command& pCmd = (*pCmdBuff)[i];

            // TODO: コマンドに応じた描画処理をする
            switch (pCmd.uType)
            {
                // TODO: 矩形を描画
                case Render::CMD_TYPE_2D_RECT:
                {
                    const Render::ComRect2D* pRect2D = &pCmd.data.rect2D;

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
                    const Render::ComText2D* pText2D = &pCmd.data.text2D;

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

    void DXLibModule::EndRender()
    {
        Render::RenderModule* pRenderModule = ModuleRender();
        HE_ASSERT(pRenderModule);

        pRenderModule->ClearCmd();

        // 画面の反映
        ScreenFlip();
    }

}  // namespace DXLib
