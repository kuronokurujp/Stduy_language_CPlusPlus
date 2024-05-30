#include "DXLibRoot.h"
#include "DxLib.h"

namespace DXLib
{
    const Bool Root::Init()
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
        }

        return bRet;
    }

    void Root::Shutdown()
    {
        // DxLibの後始末
        DxLib_End();
    }

    const Bool Root::CreateMainWindow()
    {
        return FALSE;
    }

    const Bool Root::ReleaseAllWindows()
    {
        return FALSE;
    }

    const Bool Root::BeforUpdate(const Float32 in_deltaTime)
    {
        Bool bRet = TRUE;
        {
            if ((ProcessMessage() != 0))
            {
                bRet = FALSE;
            }
            else
            {
                ClsDrawScreen();
            }
        }

        return bRet;
    }

    const Bool Root::Update(const Float32 in_deltaTime)
    {
        return TRUE;
    }

    const Bool Root::AfterUpdate(const Float32 in_deltaTime)
    {
        ScreenFlip();
        return TRUE;
    }
}
