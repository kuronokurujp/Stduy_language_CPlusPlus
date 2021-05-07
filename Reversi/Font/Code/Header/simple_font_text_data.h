#ifndef __FONT_LIB_SIMPLE_FONT_TEXT_DATA_H__
#define __FONT_LIB_SIMPLE_FONT_TEXT_DATA_H__

#include <memory>
#include <vector>
#include <algorithm>

#include "Font/Code/Header/font_text_data_interface.h"
#include "Font/Code/Header/simple_font_core_data.h"
#include "Font/Code/Header/simple_text_image_rect.h"

// テキストフォントデータ
class SimpleFontTextData : public FontTextData_Interface
{
public:
	SimpleFontTextData(
		// コアデータが常に存在しないとこのクラスは使えないので参照カウントのポイントとする
		std::shared_ptr<SimpleFontCoreData> in_r_font_core_data,
		const char* in_p_font_file_name)
	{
		this->_Clear();

		this->_shared_simple_font_core_data = in_r_font_core_data;

		FT_Error error;
		error = FT_New_Face(this->_shared_simple_font_core_data->GetLibrary(), in_p_font_file_name, 0, &this->_face);
		if (error == FT_Err_Unknown_File_Format) {
			return;
		}
		else if (error) {
			return;
		}

		this->_slot = this->_face->glyph;
	}

	~SimpleFontTextData()
	{
		if (this->_face == nullptr)
			return;

		FT_Done_Face(this->_face);
		this->_face = nullptr;
		this->_slot = nullptr;
	}

	// 使用するフォント画像のオブジェクトを作成
	std::shared_ptr<SimpleTextImageRect> NewFontImageRect(const unsigned int in_buffer_mem_size)
	{
		auto free_buffer = std::find_if(
			this->_shared_simple_text_image_rects.begin(),
			this->_shared_simple_text_image_rects.end(),
			[](std::shared_ptr<SimpleTextImageRect> v) {
			return v.use_count() == 0;
		});

		if (free_buffer == this->_shared_simple_text_image_rects.end()) {
			// 未使用のバッファが存在しないので新規作成
			std::shared_ptr<SimpleTextImageRect> add = std::make_shared<SimpleTextImageRect>(in_buffer_mem_size);
			// 末尾に新規追加
			free_buffer = this->_shared_simple_text_image_rects.insert(free_buffer, add);
		}

		return (*free_buffer);
	}

	// 作成したフォント画像からテキストを書き込む
	bool WriteFontImageTextRect(
		std::shared_ptr<SimpleTextImageRect> in_write_buffer,
		const char in_a_texts[],
		const unsigned int in_font_size)
	{
		// 縦の最大サイズは必須
		// しかし横のサイズは0でも問題ない
		FT_Set_Pixel_Sizes(this->_face, 0, in_font_size);

		in_write_buffer->NewTextImage(*this, in_a_texts, in_font_size);

		return true;
	}

	inline FT_Bitmap* LoadFontChar(unsigned long in_char_utf32) override final
	{
		FT_Load_Char(this->_face, in_char_utf32, FT_LOAD_RENDER);
		return &this->_slot->bitmap;
	}

private:
	void _Clear()
	{
		this->_face = nullptr;
		this->_slot = nullptr;
	}

	FT_Face    _face;
	FT_GlyphSlot _slot;
	std::shared_ptr<SimpleFontCoreData> _shared_simple_font_core_data;
	std::vector<std::shared_ptr<SimpleTextImageRect>> _shared_simple_text_image_rects;
};

#endif // __FONT_LIB_SIMPLE_FONT_TEXT_DATA_H__
