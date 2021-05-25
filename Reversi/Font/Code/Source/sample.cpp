#ifdef FONT_TEST_APP
/*
	フォントテキストのサンプル
*/

#include "Window/Code/Header/GUIWindow/Model/gui_window_model.h"
#include "Window/Code/Header/GUIWindow/View/Win/gui_window_win_view.h"
#include "Window/Code/Header/GUIWindow/gui_window_controller.h"

#include <memory>
#include <assert.h>

#include "Font/Code/Header/simple_font_core_data.h"
#include "Font/Code/Header/simple_font_text_data.h"
#include "Font/Code/Header/simple_text_image_rect.h"

class GUIWindowModelForTest : public GUIWindowModel
{
public:
	GUIWindowModelForTest(const int in_w, const int in_h, const char* font_file_name)
		: GUIWindowModel(in_w, in_h)
	{
		this->_count = 0;

		this->_font_core_data = std::make_shared<SimpleFontCoreData>();
		this->_p_font_text_data = new SimpleFontTextData(
			this->_font_core_data, font_file_name
		);

		this->_p_text_image_rect = this->_p_font_text_data->NewFontImageRect(320);
	}

	~GUIWindowModelForTest()
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
				32, 10);

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
	// ウィンドウサイズ
	const short screenWidth = 600;
	const short screenHeight = 400;
	const char* p_font_data_path = "Font/Roboto/Roboto-Black.ttf";

	std::shared_ptr<GUIWindowModelForTest> model =
		std::make_shared<GUIWindowModelForTest>(screenWidth, screenHeight, p_font_data_path);

	std::shared_ptr<GUIWindowWinView> view = std::make_shared<GUIWindowWinView>();

	GUIWindowController ctrl(model, view);
	if (ctrl.Start() == false)
		return -1;

	return 0;
}
#endif