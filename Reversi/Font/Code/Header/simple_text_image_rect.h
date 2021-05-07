#ifndef __FONT_LIB_SIMPLE_TEXT_IMAGE_RECT_H__
#define __FONT_LIB_SIMPLE_TEXT_IMAGE_RECT_H__

#include <codecvt>
#include <assert.h>

#include "Font/Code/Header/font_text_data_interface.h"

// テキストの矩形画像オブジェクト
class SimpleTextImageRect
{
public:
	// 色情報
	union _tag_color_union_
	{
		unsigned long gray_scale;
		struct _tag_rgba_struct_
		{
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		} rgba;
	};

	SimpleTextImageRect(const unsigned int in_buffer_mem_size = 0) {
		this->_Clear();
		// あらかじめメモリバッファを確保
		this->_CreateBuffer(in_buffer_mem_size / 2, in_buffer_mem_size / 2);
	}

	~SimpleTextImageRect()
	{
		this->_FreeBuffer();
	}

	// テキスト画像作成
	bool NewTextImage(
		FontTextData_Interface& in_r_font_text_data,
		const char in_a_texts[],
		const int in_font_size)
	{
		std::wstring_convert<std::codecvt_utf8<uint32_t>, uint32_t> utf8Str;
		auto u32str = utf8Str.from_bytes(in_a_texts);

		int num_chars = (int)u32str.size();
		int cpos_h = in_font_size;
		int cpos_w = 0;
		FT_Bitmap* p_bitmap = nullptr;
		// まずは使用するバッファサイズを計算して取得
		{
			int temp_width = 0;
			for (int n = 0; n < num_chars; ++n)
			{
				if (this->_IsNextLineCode(u32str[n])) {
					// 縦のサイズを増やす
					cpos_h += in_font_size;

					// 改行の場合一度横のサイズを暫定だが決定する
					cpos_w = cpos_w < temp_width ? temp_width : cpos_w;
					temp_width = 0;
				}
				else {
					p_bitmap = in_r_font_text_data.LoadFontChar(u32str[n]);
					temp_width += p_bitmap->width;
				}
			}

			// 横のサイズの最終決定をする
			cpos_w = cpos_w < temp_width ? temp_width : cpos_w;
		}

		// テキストバッファ作成
		{
			this->_CreateBuffer(cpos_w, cpos_h);
		}

		// テキストバッファにフォントピクセルを転送
		{
			p_bitmap = nullptr;
			cpos_w = 0;
			cpos_h = in_font_size;

			for (int n = 0; n < num_chars; ++n)
			{
				if (this->_IsNextLineCode(u32str[n])) {
					// 改行コードがあるので一段落下にする
					cpos_h += in_font_size;
					cpos_w = 0;
				}
				else {
					p_bitmap = in_r_font_text_data.LoadFontChar(u32str[n]);

					int offset_y = cpos_h - p_bitmap->rows;
					for (unsigned int i = 0; i < p_bitmap->rows * p_bitmap->width; ++i) {
						// 8bitのグレースケール値のみ取得できる

						// TODO: 色付け対応が別途必要だが未対応
						// 白・黒の2色のみ設定
						const unsigned char c = p_bitmap->buffer[i];
						this->_TranslateBuffer(
							(i % p_bitmap->width) + cpos_w, offset_y + (i / p_bitmap->width),
							c, c, c, c);
					}

					cpos_w += p_bitmap->width;
				}
			}
		}

		return true;
	}

	inline unsigned int Width() const { return this->_w; }
	inline unsigned int Height() const { return this->_h; }

	inline _tag_color_union_ GetBuffer(unsigned int x, unsigned int y) {
		return this->_colors[(this->_w * y) + x];
	}

private:
	void _Clear()
	{
		this->_colors = NULL;
		this->_w = this->_h = 0;
		this->_mem_size = 0;
	}

	inline bool _IsNextLineCode(const uint32_t in_char_code) {
		return (in_char_code == '\n');
	}

	void _CreateBuffer(const unsigned int in_w, const unsigned int in_h)
	{
		// 引数の値が0を許容する
		// 0を許容した結果メモリが確保されずに処理が終わるケースがある

		this->_w = in_w;
		this->_h = in_h;
		unsigned int new_mem_size = this->_w * this->_h;
		// 新しいメモリサイズがすでに確保した古いメモリサイズより大きい場合はメモリ確保しなおす
		if (this->_mem_size < new_mem_size) {
			// メモリ確保していない場合は0から確保
			if (this->_colors == NULL) {
				this->_colors =
					reinterpret_cast<_tag_color_union_*>(
						::malloc(this->_w * this->_h * sizeof(_tag_color_union_)));
			}
			// すでにメモリ確保している場合は確保領域を増やす
			else {
				this->_colors =
					reinterpret_cast<_tag_color_union_*>(
						::realloc(this->_colors, this->_w * this->_h * sizeof(_tag_color_union_)));
			}
			this->_mem_size = new_mem_size;
		}

		if (this->_colors != NULL)
			::memset(this->_colors, 0, sizeof(_tag_color_union_) * this->_w * this->_h);
	}

	void _FreeBuffer()
	{
		if (this->_colors == NULL)
			return;

		::free(this->_colors);

		this->_colors = NULL;

		this->_w = this->_h = 0;
		this->_mem_size = 0;
	}

	inline void _TranslateBuffer(
		const unsigned int in_x, const unsigned int in_y,
		const unsigned char in_r, const unsigned char in_g, const unsigned char in_b,
		const unsigned char in_a)
	{
		assert(this->_colors != nullptr);
		_tag_color_union_* p = &this->_colors[(in_y * this->_w) + in_x];
		p->rgba.r = in_r;
		p->rgba.g = in_g;
		p->rgba.b = in_b;
		p->rgba.a = in_a;
	}

	_tag_color_union_* _colors;
	unsigned int _w, _h;
	unsigned int _mem_size;
};
#endif // __FONT_LIB_SIMPLE_TEXT_IMAGE_RECT_H__
