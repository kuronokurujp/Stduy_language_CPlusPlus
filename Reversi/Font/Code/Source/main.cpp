#ifdef FONT_TEST_APP

#include "Window/Code/Header/GUIWindow/Model/gui_window_model.h"
#include "Window/Code/Header/GUIWindow/View/Win/gui_window_win_view.h"
#include "Window/Code/Header/GUIWindow/gui_window_controller.h"

#include <memory>
#include <assert.h>

#include "Font/Code/Header/simple_font_core_data.h"
#include "Font/Code/Header/simple_font_text_data.h"
#include "Font/Code/Header/simple_text_image_rect.h"

class GUIWindowModelForTest2 : public GUIWindowModel
{
public:
	GUIWindowModelForTest2(const int in_w, const int in_h, const char* font_file_name)
		: GUIWindowModel(in_w, in_h)
	{
		FT_Init_FreeType(&library);
		FT_New_Face(library, font_file_name, 0, &face);
		slot = face->glyph;
		// size
		int font_size = 32;
		//FT_Set_Char_Size(face, 0, 16 * font_size, 128, 128);
		//FT_Set_Char_Size(face, font_size, font_size, 0, 0);
		// 縦の最大サイズは必須
		// しかし横のサイズは0でも問題ない
		FT_Set_Pixel_Sizes(face, 0, 32);
	}

	~GUIWindowModelForTest2()
	{
	}

	// 描画バッファを更新
	void UpdateRender() override
	{
		auto p = this->GetCurrentFrameBuffer();
		char text[] = u8"Hello, world!";
		//char text[] = u8"test! aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

		std::wstring_convert<std::codecvt_utf8<uint32_t>, uint32_t> utf8Str;
		auto u32str = utf8Str.from_bytes(text);

		//		int num_chars = (int)strlen(text);
		int num_chars = (int)u32str.size();
		{
			// テキスト位置
			int cpos_x = 0;
			int cpos_y = 0;

			//int cpos_h = 16;
			int cpos_h = 32;

			int pen_x, pen_y, n;
			pen_x = 0;
			pen_y = 0;
			for (n = 0; n < num_chars; n++) {
				int i;
				FT_Bitmap bitmap;

				FT_Load_Char(face, u32str[n], FT_LOAD_RENDER);
				bitmap = slot->bitmap;

				// FT_Set_Char_Sizeで設定したレゾリューションでbitmap.rowsの値が変わる
				// 文字を下揃えにしている
				// これがないと濁点などが上にきて不自然になる
				int offset_y = cpos_h - bitmap.rows + cpos_y;
				for (i = 0; i < bitmap.rows * bitmap.width; i++) {
					// 8bitのグレースケール値のみ取得できる
					// 色どう付けつけるかは別途対応が必要
					p->SetPixel(
						(i % bitmap.width) + cpos_x, offset_y + (i / bitmap.width),
						bitmap.buffer[i], bitmap.buffer[i], bitmap.buffer[i]);
				}

				cpos_x += bitmap.width + 5;
				pen_x += slot->advance.x >> 6;
			}
		}

		GUIWindowModel::UpdateRender();
	}
	FT_Library library;
	FT_Face    face;
	FT_GlyphSlot slot;
	FT_UInt glyph_index;
	FT_Error error;
};

class GUIWindowModelForTest3 : public GUIWindowModel
{
public:
	GUIWindowModelForTest3(const int in_w, const int in_h, const char* font_file_name)
		: GUIWindowModel(in_w, in_h)
	{
		this->_count = 0;

		this->_font_core_data = std::make_shared<SimpleFontCoreData>();
		this->_p_font_text_data = new SimpleFontTextData(
			this->_font_core_data, font_file_name
		);

		this->_p_text_image_rect = this->_p_font_text_data->NewFontImageRect(320);
	}

	~GUIWindowModelForTest3()
	{
		this->_p_text_image_rect = nullptr;

		if (this->_p_font_text_data != nullptr)
			delete this->_p_font_text_data;
	}

	// 描画バッファを更新
	void UpdateRender() override
	{
		auto p = this->GetCurrentFrameBuffer();
		p->Cls(128);

		// 更新のたびにテキストを変えてみる
		{
			std::string text = u8"Count";
			text += std::to_string(this->_count);
			this->_p_font_text_data->WriteFontImageTextRect(
				this->_p_text_image_rect,
				text.c_str(),
				32
			);

			this->_count += 1;
			this->_count %= 256;
		}

		{
			for (unsigned int y = 0; y < this->_p_text_image_rect->Height(); ++y)
			{
				for (unsigned int x = 0; x < this->_p_text_image_rect->Width(); ++x)
				{
					// 転送しないクリップする色ならスキップ
					auto c = this->_p_text_image_rect->GetBuffer(x, y).gray_scale;
					if (c == 0)
						continue;

					p->SetPixel(x, y, c);
				}
			}
		}

		GUIWindowModel::UpdateRender();
	}
private:
	std::shared_ptr<SimpleFontCoreData> _font_core_data;
	SimpleFontTextData* _p_font_text_data;
	std::shared_ptr<SimpleTextImageRect> _p_text_image_rect;

	int _count;
};

int main()
{
	// ゲーム用にレンダリングするモデルを作成
	const short rendererScreenWidth = 600;
	const short rendererScreenHeight = 400;
	const char* p_font_data_path = "Font/Roboto/Roboto-Black.ttf";

	/*
	std::shared_ptr<GUIWindowModelForTest2> model =
		std::make_shared<GUIWindowModelForTest2>(rendererScreenWidth, rendererScreenHeight, p_font_data_path);
		*/
	std::shared_ptr<GUIWindowModelForTest3> model =
		std::make_shared<GUIWindowModelForTest3>(rendererScreenWidth, rendererScreenHeight, p_font_data_path);

	std::shared_ptr<GUIWindowWinView> view = std::make_shared<GUIWindowWinView>();

	GUIWindowController ctrl(model, view);
	if (ctrl.Start() == false)
		return -1;

	return 0;
}
#endif