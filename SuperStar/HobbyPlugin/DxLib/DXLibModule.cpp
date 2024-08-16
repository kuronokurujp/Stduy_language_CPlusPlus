#include "DXLibModule.h"

#include "DxLib.h"

// 依存モジュール一覧
#include "RenderModule.h"

namespace DXLib
{
    DXLibModule::DXLibModule() : PlatformModule()
    {
        this->_AppendDependenceModule<Render::RenderModule>();
    }

    const Bool DXLibModule::_Start()
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

            // 初期化と同時にウィンドウが表示するので非表示にする
            // 表示はウィンドウ生成メソッドが呼ばれたら
            // ウィンドウを非表示にするとアプリが非アクティブになってしまうので
            // 一時的に非アクティブでも動作するようにします
            SetAlwaysRunFlag(TRUE);

            // ウィンドウを非表示にします
            SetWindowVisibleFlag(FALSE);

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

    const Bool DXLibModule::_Release()
    {
        // DxLibの後始末
        DxLib_End();

        return TRUE;
    }

    const Bool DXLibModule::CreateMainWindow()
    {
        // ウィンドウを表示する
        SetWindowVisibleFlag(TRUE);

        // 非アクティブでも実行する設定を解除します
        SetAlwaysRunFlag(FALSE);

        return TRUE;
    }

    const Bool DXLibModule::ReleaseAllWindows()
    {
        return FALSE;
    }

    const Bool DXLibModule::_BeforeUpdate(const Float32 in_fDeltaTime)
    {
        {
            // 入力更新
            this->_input.Update(in_fDeltaTime);

            // 0以外だと画面終了メッセージとみなしている
            if ((ProcessMessage() != 0))
            {
                this->_bQuit = TRUE;
            }
        }

        return (this->_bQuit == FALSE);
    }

    const Bool DXLibModule::_Update(const Float32 in_fDeltaTime)
    {
        if (this->_bQuit) return FALSE;

        // 画面の描画クリア
        ClearDrawScreen();

        auto pRenderModule = this->GetDependenceModule<Render::RenderModule>();
        HE_ASSERT(pRenderModule);

        Render::CommandBuffer* pCmdBuff =
            reinterpret_cast<Render::CommandBuffer*>(pRenderModule->GetCmdBuff());
        HE_ASSERT(pCmdBuff);

        for (Uint32 i = 0; i < pCmdBuff->Size(); ++i)
        {
            const Render::Command& pCmd = (*pCmdBuff)[i];

            // コマンドに応じた描画処理をする
            switch (pCmd.uType)
            {
                // 矩形を描画
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
                // 2Dテキストを描画
                case Render::CMD_TYPE_2D_TEXT:
                {
                    const Render::ComText2D* pText2D = &pCmd.data.text2D;

                    Uint32 uColor =
                        GetColor(pText2D->color.c32.r, pText2D->color.c32.g, pText2D->color.c32.b);
                    Core::Common::FixString1024 str(pText2D->szChars);
                    int x = static_cast<int>(pText2D->fX);
                    int y = static_cast<int>(pText2D->fY);
                    // 配置指定で配置座標を変更
                    switch (pText2D->anchor)
                    {
                        // 中央位置
                        case Core::Math::Rect2::EAnchor_Center:
                        {
                            const int sWidth =
                                GetDrawStringWidth(pText2D->szChars, HE_STR_LEN(pText2D->szChars));
                            const int sHeight = GetDrawStringWidth(pText2D->szChars, 1);
                            if ((sWidth != -1) && (sHeight != -1))
                            {
                                x -= sWidth / 2;
                                y -= sHeight / 2;
                            }
                            break;
                        }
                        // 左上位置
                        case Core::Math::Rect2::EAnchor_Left:
                        {
                            break;
                        }
                    }

                    DrawString(x, y, pText2D->szChars, uColor);
                    break;
                }
                default:
                    HE_ASSERT(0 && "存在しないコマンド");
            }
        }
        return TRUE;
    }

    const Bool DXLibModule::_LateUpdate(const Float32 in_fDeltaTime)
    {
        if (this->_bQuit) return FALSE;

        auto pRenderModule = this->GetDependenceModule<Render::RenderModule>();
        HE_ASSERT(pRenderModule);

        pRenderModule->ClearCmd();

        // 画面の反映
        ScreenFlip();

        return TRUE;
    }

}  // namespace DXLib
