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

		this->ResizeTempBuffer(in_buffer_mem_size);
	}

	~SimpleTextImageRect() {
		this->_FreeBuffer();
	}

	/// <summary>
	/// 作業用バッファのサイズを変える.
	/// </summary>
	/// <param name="in_buffer_mem_size">Size of the in buffer memory.</param>
	void ResizeTempBuffer(const unsigned int in_buffer_mem_size)
	{
		// 作業用バッファサイズ値があればあらかじめバッファを確保
		if (0 < in_buffer_mem_size)
			this->_CreateBuffer(in_buffer_mem_size / 2, in_buffer_mem_size / 2);
	}

	// テキスト画像作成
	bool CreateTextImage(
		FontTextData_Interface& in_r_font_text_data,
		const char in_a_texts[],
		const unsigned int in_font_size,
		// 文字と文字のスペースサイズ
		const unsigned int in_char_space_size)
	{
		// 文字列の中に0があればそこを終端とする
		std::wstring_convert<std::codecvt_utf8<uint32_t>, uint32_t> utf8Str;
		auto u32str = utf8Str.from_bytes(in_a_texts);

		int num_chars = (int)u32str.size();
		int cpos_h = in_font_size;
		int cpos_w = 0;
		FT_Bitmap* p_bitmap = nullptr;
		// まずは使用するバッファサイズを計算して取得
		// 引数で指定したフォントサイズを基準にしている
		// ※ ビットマップロードした時にビットマップサイズがフォントサイズを超えるとバグる
		//    FreeType2について知見が少ないので上記のケースが起きるのか起きないのか分からない
		{
			int temp_width = 0;
			// メモリバッファのサイズ基準をフォントサイズの2倍にする
			// 1文字の文字の縦横サイズがフォントサイズを超えるから
			int buffer_base_size = in_font_size * 2;
			for (int n = 0; n < num_chars; ++n)
			{
				if (this->_IsNextLineCode(u32str[n])) {
					// 縦のサイズを増やす
					cpos_h += buffer_base_size;

					// 改行の場合一度横のサイズを暫定だが決定する
					cpos_w = cpos_w < temp_width ? temp_width : cpos_w;
					temp_width = 0;
				}
				else {
					temp_width += buffer_base_size;
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

			int offset_y = 0;
			unsigned int pixel_data_count = 0;
			for (int n = 0; n < num_chars; ++n)
			{
				if (this->_IsNextLineCode(u32str[n])) {
					// 改行コードがあるので一段落下にする
					cpos_h += in_font_size;
					cpos_w = 0;
				}
				else {
					p_bitmap = in_r_font_text_data.LoadFontChar(u32str[n]);

					offset_y = cpos_h - p_bitmap->rows;
					pixel_data_count = p_bitmap->rows * p_bitmap->width;
					for (unsigned int i = 0; i < pixel_data_count; ++i) {
						// 8bitのグレースケール値のみ取得できる

						// TODO: 色付け対応が別途必要だが未対応
						// 白・黒の2色のみ設定
						const unsigned char c = p_bitmap->buffer[i];
						this->_TranslateBuffer(
							(i % p_bitmap->width) + cpos_w, offset_y + (i / p_bitmap->width),
							c, c, c, c);
					}

					cpos_w += this->_CalcCharWidth(p_bitmap, in_font_size, in_char_space_size);
				}
			}

			this->_font_w = cpos_w;
			this->_font_h = cpos_h;
		}

		this->_write_char_num = num_chars;

		return true;
	}

	inline unsigned int Width() const { return this->_w; }
	inline unsigned int Height() const { return this->_h; }

	inline unsigned int FontWidth() const { return this->_font_w; }
	inline unsigned int FontHeight() const { return this->_font_h; }

	inline unsigned int GetWriteCharNum() const { return this->_write_char_num; }

	inline _tag_color_union_ GetBuffer(unsigned int x, unsigned int y) {
		return this->_colors[(this->_w * y) + x];
	}

private:
	void _Clear()
	{
		this->_colors = NULL;
		this->_w = this->_h = 0;
		this->_mem_size = 0;
		this->_write_char_num = 0;
	}

	inline bool _IsNextLineCode(const uint32_t in_char_code) {
		return (in_char_code == '\n');
	}

	inline unsigned int _CalcCharWidth(
		FT_Bitmap* in_p_bitmap,
		const unsigned int in_font_size,
		const unsigned int in_space_size)
	{
		if (in_p_bitmap->width <= 0)
			return in_space_size + in_font_size;

		return in_p_bitmap->width + in_space_size;
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

		this->_w = this->_h = this->_font_w = this->_font_h = 0;
		this->_mem_size = 0;

		this->_write_char_num = 0;
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
	unsigned int _w, _h, _font_w, _font_h;
	unsigned int _mem_size;
	unsigned int _write_char_num;
};
#endif // __FONT_LIB_SIMPLE_TEXT_IMAGE_RECT_H__
