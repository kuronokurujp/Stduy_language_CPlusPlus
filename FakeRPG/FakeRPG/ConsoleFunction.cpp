#include "ConsoleFunction.h"

#include <Windows.h>
#include <WinUser.h>
#include <cwchar>
#include <locale.h>
#include <stdlib.h>

#pragma comment(lib, "user32")

namespace Console
{
    static HANDLE s_handle = nullptr;

    /// <summary>
    /// 画面クリア
    /// </summary>
    void Cls()
    {
        system("cls");
    }

    /// <summary>
    /// コンソールハンドルを取得
    /// </summary>
    /// <returns></returns>
    const HANDLE GetHandle()
    {
        if (s_handle == nullptr)
            s_handle = GetStdHandle(STD_OUTPUT_HANDLE);

        return s_handle;
    }

    /// <summary>
    /// カーソル表示設定
    /// </summary>
    /// <param name="in_enable"></param>
    void SetDisplayCursor(bool in_enable)
    {
        const CONSOLE_CURSOR_INFO cursor{ 1, in_enable };
        SetConsoleCursorInfo(GetHandle(), &cursor);
    }

    /// <summary>
    /// カーソル位置設定
    /// </summary>
    /// <param name="in_x"></param>
    /// <param name="in_y"></param>
    void SetPositionCursor(const short in_x, const short in_y)
    {
        SetConsoleCursorPosition(GetHandle(), COORD{ in_x, in_y });
    }

    /// <summary>
    /// コンソールをフルスクリーンにする
    /// 参考サイト
    /// https://stackoverflow.com/questions/4053554/running-a-c-console-program-in-full-screen
    /// </summary>
    void SwitchFullScreenMode(const bool in_enable)
    {
        if (in_enable)
            ::SendMessage(::GetConsoleWindow(), WM_SYSKEYDOWN, VK_RETURN, 0x20000000);
    }

    /// <summary>
    /// コンソールウィンドウサイズの変更
    /// 参考サイト
    ///     https://nodachisoft.com/common/jp/article/jp000137/
    /// </summary>
    /// <param name="in_handle"></param>
    /// <param name="in_width"></param>
    /// <param name="in_height"></param>
    /// <returns></returns>
    const bool SetScreenSize(const short in_width, const short in_height)
    {
        SMALL_RECT console_rect = { 0, 0, 1, 1 };

        HANDLE handle = GetHandle();
        // 初期サイズに
        SetConsoleWindowInfo(handle, TRUE, &console_rect);

        COORD coord = { in_width, in_height };
        if (SetConsoleScreenBufferSize(handle, coord))
            return false;

        console_rect.Right = in_width - 1;
        console_rect.Bottom = in_height - 1;
        SetConsoleWindowInfo(handle, FALSE, &console_rect);

        return true;
    }

    /// <summary>
    /// フォントサイズ設定
    /// 参考サイト
    /// https://stackoverflow.com/questions/35382432/how-to-change-the-console-font-size
    /// </summary>
    /// <param name="in_size"></param>
    void SetFontSize(const short in_size)
    {
        // コンソールのフォント設定
        CONSOLE_FONT_INFOEX cfi;
        cfi.cbSize = sizeof(cfi);
        cfi.nFont = 0;
        cfi.dwFontSize.X = 0;
        cfi.dwFontSize.Y = in_size;
        cfi.FontFamily = FF_DONTCARE;
        cfi.FontWeight = FW_NORMAL;

        wcscpy_s(cfi.FaceName, sizeof(cfi.FaceName) / sizeof(cfi.FaceName[0]), L"Consolas");
        SetCurrentConsoleFontEx(GetHandle(), FALSE, &cfi);
    }
}
