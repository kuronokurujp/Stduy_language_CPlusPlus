#ifndef __BMP_FORMAT_FRAME_BUFFER_H__
#define __BMP_FORMAT_FRAME_BUFFER_H__

#include "Window/Code/Header/Render/frame_buffer.h"

/// <summary>
/// BMPフォーマットの画面フレームバッファ
/// BMPなので画面左下が原点になり右上が最大点となるので注意
/// BMPだと画素の左下が原点になる。
/// スクリーン座標の左上原点で扱うための座標系の違うを吸収するクラス
/// </summary>
/// <seealso cref="FrameBuffer" />
class BMPFormatFrameBuffer : public FrameBuffer
{
public:
	BMPFormatFrameBuffer(const int in_width, const int in_height) : FrameBuffer(in_width, in_height) {}

	/// <summary>
	/// 画素のアドレス取得.
	/// スクリーン座標系を基準とした原点左上に変換してアドレスを返す
	/// </summary>
	/// <param name="in_x">The in x.</param>
	/// <param name="in_y">The in y.</param>
	/// <returns></returns>
	inline unsigned long* GetAddressPixel(const int in_x, const int in_y) override final {
		auto y = this->_height - 1 - in_y;
		return (this->_width * y + in_x) + this->_p_buffer;
	}

	inline const unsigned long* GetAddressPixel(const int in_x, const int in_y) const override final {
		auto y = this->_height - 1 - in_y;
		return (this->_width * y + in_x) + this->_p_buffer;
	}
};
#endif // __BMP_FORMAT_FRAME_BUFFER_H__
