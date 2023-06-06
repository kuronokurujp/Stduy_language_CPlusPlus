#pragma once

#include <Windows.h>
#include <conio.h>

namespace Console
{
    /// <summary>
    /// 画面クリア
    /// </summary>
    extern void Cls();

    /// <summary>
    /// コンソールハンドルを取得
    /// </summary>
    /// <returns></returns>
    extern const HANDLE GetHandle();

    /// <summary>
    /// カーソル表示設定
    /// </summary>
    /// <param name="in_enable"></param>
    extern void SetDisplayCursor(bool in_enable);

    /// <summary>
    /// カーソル位置設定
    /// </summary>
    /// <param name="in_x"></param>
    /// <param name="in_y"></param>
    extern void SetPositionCursor(const short in_x, const short in_y);

    /// <summary>
    /// コンソールをフルスクリーンにする
    /// </summary>
    extern void SwitchFullScreenMode(const bool in_enable);

    /// <summary>
    /// コンソールウィンドウサイズの変更
    /// 参考サイト
    ///     https://nodachisoft.com/common/jp/article/jp000137/
    /// </summary>
    /// <param name="in_handle"></param>
    /// <param name="in_width"></param>
    /// <param name="in_height"></param>
    /// <returns></returns>
    extern const bool SetScreenSize(const short in_width, const short in_height);

    /// <summary>
    /// フォントサイズ設定
    /// </summary>
    /// <param name="in_size"></param>
    extern void SetFontSize(const short in_size);
}
