#ifndef __WINDOW_FRAMBUFFER_H__
#define __WINDOW_FRAMBUFFER_H__

#include "Render/frame_buffer.h"
#include <windows.h>

/// <summary>
/// BMPフォーマットの画面フレームバッファを作成
/// BMPなので画面左下が原点になり右上が最大点となるので注意
/// </summary>
/// <seealso cref="FrameBuffer" />
class WindowFrameBuffer : public FrameBuffer
{
public:
	WindowFrameBuffer(const int in_width, const int in_height);

	/// <summary>
	/// バッファのリサイズをする.
	/// </summary>
	/// <param name="in_width">Width of the in.</param>
	/// <param name="in_height">Height of the in.</param>
	/// <returns></returns>
	bool Resize(const int in_width, const int in_height) override;

	/// <summary>
	/// フレームバッファを書き込む.
	/// </summary>
	/// <param name="in_p_frame_buffer">The in p frame buffer.</param>
	void Flash(FrameBuffer* in_p_frame_buffer);

	/// <summary>
	/// ウィンドウ描画.
	/// </summary>
	/// <param name="in_handle">The in handle.</param>
	/// <param name="in_x">The in x.</param>
	/// <param name="in_y">The in y.</param>
	void Display(HWND in_handle, const int in_x, const int in_y);
	void Display(
		HWND in_handle,
		const int in_x, const int in_y,
		const int in_sx, const int in_sy,
		const int in_sw, const int in_sh);

protected:
	void _FreeBuffer() override;

private:
	HBITMAP _h_bitmap;
	HWND _h_wnd;
	HDC _h_dc;
	HDC _h_desk_dc;
	HGDIOBJ _h_old_handle;
};

#endif // !__WINDOW_FRAMBUFFER_H__
