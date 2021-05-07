#include "Window/Code/Header/GUIWindow/Model/window_frame_buffer.h"

#include <stdio.h>
#include <assert.h>

WindowFrameBuffer::WindowFrameBuffer(const int in_width, const int in_height)
	: FrameBuffer()
{
	this->_h_bitmap = NULL;
	this->_h_dc = NULL;
	this->_h_old_handle = NULL;

	this->Resize(in_width, in_height);
	this->Cls();
}

bool WindowFrameBuffer::Resize(const int in_width, const int in_height)
{
	this->_FreeBuffer();

	this->_width = in_width;
	this->_height = in_height;
	this->_pixel_byte_size = 4;
	// バッファのサイズ計算
	this->_buffer_size = this->_width * this->_height * this->_pixel_byte_size;

	// bitmapのデータ構造詳細
	// https://docs.microsoft.com/en-us/previous-versions/dd183376(v=vs.85)
	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(bmi));
	{
		BITMAPINFOHEADER* p_bmi_header = &bmi.bmiHeader;
		p_bmi_header->biSize = sizeof(BITMAPINFOHEADER);
		p_bmi_header->biWidth = this->_width;
		p_bmi_header->biHeight = this->_height;
		// ここはなぜか1にしないとだめ
		p_bmi_header->biPlanes = 1;
		// ピクセルは32byte(2の32乗)
		// ビット単位で設定
		p_bmi_header->biBitCount = (WORD)(this->_pixel_byte_size * 8);
		// 非圧縮形式
		p_bmi_header->biCompression = BI_RGB;
	}

	HWND desk_top_hwnd = ::GetDesktopWindow();
	HDC tmp_dc = ::GetDC(desk_top_hwnd);

	// ビットマップデータを作成して作成したバッファデータのアドレスを取得
	this->_h_bitmap = CreateDIBSection(tmp_dc, &bmi, DIB_RGB_COLORS, (void**)&this->_p_buffer, 0, 0);
	if (this->_h_bitmap == NULL)
	{
		// 作成に失敗
		::ReleaseDC(desk_top_hwnd, tmp_dc);
		return false;
	}

	// バッファデータの結びつけ
	this->_h_dc = ::CreateCompatibleDC(tmp_dc);
	this->_h_old_handle = ::SelectObject(this->_h_dc, this->_h_bitmap);

	::ReleaseDC(desk_top_hwnd, tmp_dc);

	return true;
}

void WindowFrameBuffer::Flash(FrameBuffer * in_p_frame_buffer)
{
	::memcpy(this->_p_buffer, in_p_frame_buffer->Buffer(), this->_buffer_size);
}

/// <summary>
/// 画面全体を書き込む.
/// </summary>
/// <param name="in_handle">The in handle.</param>
/// <param name="in_x">The in x.</param>
/// <param name="in_y">The in y.</param>
void WindowFrameBuffer::Display(HWND in_handle, const int in_x, const int in_y)
{
	this->Display(in_handle, in_x, in_y, 0, 0, this->_width, this->_height);
}

/// <summary>
/// 指定した矩形領域を書き換えて.
/// </summary>
/// <param name="in_handle">The in handle.</param>
/// <param name="in_x">The in x.</param>
/// <param name="in_y">The in y.</param>
/// <param name="in_sx">The in sx.</param>
/// <param name="in_sy">The in sy.</param>
/// <param name="in_sw">The in sw.</param>
/// <param name="in_sh">The in sh.</param>
void WindowFrameBuffer::Display(
	HWND in_handle,
	const int in_x, const int in_y,
	const int in_sx, const int in_sy,
	const int in_sw, const int in_sh)
{
	HDC desk_dc = ::GetDC(in_handle);

	::BitBlt(desk_dc, in_x, in_y, in_sw, in_sh, this->_h_dc, in_sx, in_sy, SRCCOPY);

	::ReleaseDC(in_handle, desk_dc);
}

void WindowFrameBuffer::_FreeBuffer()
{
	if (this->_p_buffer == NULL)
		return;

	::SelectObject(this->_h_dc, this->_h_old_handle);
	this->_h_old_handle = NULL;

	::DeleteDC(this->_h_dc);
	this->_h_dc = NULL;

	::DeleteObject(this->_h_bitmap);
	this->_h_bitmap = NULL;

	this->_p_buffer = NULL;
}