#pragma once

#include "DXLibScreen.h"

#include "DxLib.h"

namespace DXLib
{
    const Bool Screen::Init()
    {
        // WindowModeにしている
        ::ChangeWindowMode(TRUE);
        // ウィンドウサイズを自由に変える事はしない
        ::SetWindowSizeChangeEnableFlag(FALSE);

        // TODO: ウィンドウサイズは適当
        {
            this->_w = 640.0f;
            this->_h = 480.0f;
            auto result =
                ::SetGraphMode(static_cast<int>(this->_w), static_cast<int>(this->_h), 32);
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
        ::SetDrawScreen(DX_SCREEN_BACK);

        return TRUE;
    }

    const Uint32 Screen::VWidth() const
    {
        return this->_w;
    }

    const Uint32 Screen::VHeight() const
    {
        return this->_h;
    }
}  // namespace DXLib
