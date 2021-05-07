#ifdef __CUI_GAME__

#include "System/Platform/CUI/console_renderer.h"
#include "System/Platform/CUI/keyboard.h"

#else

#include <thread>
#include <vector>

#include "System/Platform/Win32/win32_frame_renderer.h"
#include "System/Platform/CUI/keyboard.h"

// GUIウィンドウ
#include "Window/Code/Header/GUIWindow/Model/gui_window_model.h"
#include "Window/Code/Header/GUIWindow/View/Win/gui_window_win_view.h"
#include "Window/Code/Header/GUIWindow/gui_window_controller.h"

#include "Window/Code/Header/Common/utility.h"
#include "Window/Code/Header/Common/color.h"

#include "Window/Code/Header/Math/vec3.h"
#include "Window/Code/Header/Math/ray.h"

// フォント描画
#include "Font/Code/Header/simple_font_core_data.h"
#include "Font/Code/Header/simple_font_text_data.h"
#include "Font/Code/Header/simple_text_image_rect.h"

#endif

#include "common.h"

#include "game_controller.h"
#include "System/fps.h"

#ifdef __CUI_GAME__
#else

// ゲーム専用のGUIウィンドウ専用モデル
class GUIWindowModelForGame : public GUIWindowModel
{
public:
	GUIWindowModelForGame(Win32FrameRenderer* in_p_renderer)
		: GUIWindowModel(in_p_renderer->GetTextRenderWidth(), in_p_renderer->GetTextRenderHeight())
	{
		this->_p_renderer = in_p_renderer;
		int width = this->_p_renderer->GetTextRenderWidth();
		int height = this->_p_renderer->GetTextRenderHeight();

		const double d_width = width - 1;
		const double d_height = height - 1;

		this->_inv_d_width = 1.0 / d_width;
		this->_inv_d_heigth = 1.0 / d_height;

		// フォントテキストデータを作成
		{
			const char* p_font_file_name = "Font/Roboto/Roboto-Black.ttf";

			this->_font_core_data = std::make_shared<SimpleFontCoreData>();
			this->_p_font_text_data = new SimpleFontTextData(
				this->_font_core_data, p_font_file_name
			);

			this->_p_text_image_rect = this->_p_font_text_data->NewFontImageRect(320);
		}
	}

	~GUIWindowModelForGame()
	{
		{
			this->_p_text_image_rect = nullptr;

			if (this->_p_font_text_data != nullptr)
				delete this->_p_font_text_data;
			this->_p_font_text_data = nullptr;
		}
	}

	// 描画バッファを更新
	void UpdateRender() override
	{
		auto p_frame_buffer = this->_p_frame_buffer[this->_frame_buffer_count];
		p_frame_buffer->Cls(128);

		// レイトレース空間の情報から画面に表示するピクセルを取得してフレームバッファに書き込む
		{
			int w0 = 0;
			int w1 = 0;
			int h0 = 0;
			int h1 = 0;
			int thread_max = std::thread::hardware_concurrency();
			int width = this->_p_renderer->GetTextRenderWidth();
			w1 = width;
			int height = this->_p_renderer->GetTextRenderHeight() / thread_max;

			std::vector<std::thread> threads;
			for (int i = 0; i < thread_max; ++i) {
				// 書き込むと高さの始めと終わりの位置を計算
				h0 = height * i;
				h1 = height * (i + 1);

				threads.emplace_back(
					[w0, w1, h0, h1, this]() {
					// 読み取りがメインで書き込み先は競合しないのでロックはしない

					_Update(w0, w1, h0, h1 - 1);
				});
			}

			for (auto& t : threads) {
				t.join();
			}
		}

		// テキストフォントを表示
		{
			{
				std::string text = u8"Count";
				this->_p_font_text_data->WriteFontImageTextRect(
					this->_p_text_image_rect,
					text.c_str(),
					24
				);
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

						p_frame_buffer->SetPixel(x, y, c);
					}
				}
			}
		}

		GUIWindowModel::UpdateRender();
	}

private:
	void _Update(const int in_w0, const int in_w1, const int in_h0, const int in_h1)
	{
		auto p_frame_buffer = this->_p_frame_buffer[this->_frame_buffer_count];
		auto p_space = this->_p_renderer->GetRayTraceSpacePtr();

		int ir, ig, ib;
		ir = ig = ib = 0;

		double u, v;
		Ray ray;
		Color color;
		Color temp_color;
		// 100位にしないときれいにならない
		// でもそうすると負荷がでかくてFPSが激落ちするのでいったんアンチエイリアスはやらない
		const int sample_per_pixel = 1;
		const int max_depth = 1;

		// 横ラインを先に書き込む
		for (int y = in_h1; y >= in_h0; --y)
		{
			double d_y = double(y);
			v = d_y * this->_inv_d_heigth;

			for (int x = in_w0; x < in_w1; ++x)
			{
				color.Zero();
				temp_color.Zero();
				double d_x = double(x);

				{
					u = d_x * this->_inv_d_width;

					p_space->_camera.OutputRay(&ray, u, v);
					p_space->OutputRayColor(&color, ray, max_depth, max_depth);
				}

				{
					ir = ColorUtility::ConverRGB01ToRGB255(color.x());
					ig = ColorUtility::ConverRGB01ToRGB255(color.y());
					ib = ColorUtility::ConverRGB01ToRGB255(color.z());
				}

				p_frame_buffer->SetPixel(x, y, ir, ig, ib);
			}
		}
	}

private:
	Win32FrameRenderer* _p_renderer;
	double _inv_d_width;
	double _inv_d_heigth;

	std::shared_ptr<SimpleFontCoreData> _font_core_data;
	SimpleFontTextData* _p_font_text_data;
	std::shared_ptr<SimpleTextImageRect> _p_text_image_rect;
};

// ゲーム用のGUIウィンドウコントローラー
class GUIWindowControllerForGame : public GUIWindowController
{
public:
	// 制御モデルは外部から受け取る
	GUIWindowControllerForGame(
		std::shared_ptr<GUIWindowModel> in_model,
		std::shared_ptr<GUIWindowWinView> in_view,
		GameController* in_p_game_ctrl)
		: GUIWindowController(in_model, in_view)
	{
		assert(in_p_game_ctrl != nullptr);
		this->_p_game_ctrl = in_p_game_ctrl;
	}

	// 更新
	void Update() override final
	{
		auto deltaTimeSecond = this->_fps.WaitUpdate();
		this->_p_game_ctrl->Update(deltaTimeSecond);
	}

	// 描画
	void Render() override final
	{
		this->_p_game_ctrl->Render();
		// ここでレイトレースを描画反映している
		GUIWindowController::Render();
	}

private:
	Fps _fps;
	GameController* _p_game_ctrl;
};

#endif

/// <summary>
/// プログラムメイン.
/// </summary>
/// <returns></returns>
int main(int argc, const char * argv[])
{
	// 描画レンダーの初期化
#ifdef __CUI_GAME__
	const short rendererScreenWidth = 100;
	const short rendererScreenHeight = 40;
	ConsoleRenderer::sSetup rendererSetup(rendererScreenWidth, rendererScreenHeight);
	ConsoleRenderer renderer(rendererSetup);

	Keyboard keyboard;

	Fps fps;

	GameController gameController(&renderer, &keyboard);
	float deltaTimeSecond = 0.0f;

	while (gameController.IsPlaying())
	{
		deltaTimeSecond = fps.WaitUpdate();

		keyboard.Input();

		gameController.Update(deltaTimeSecond);
		gameController.Render();
	}
#else

	// ゲーム用にレンダリングするモデルを作成
	const short rendererScreenWidth = 600;
	const short rendererScreenHeight = 400;
	auto p_renderer = new Win32FrameRenderer(rendererScreenWidth, rendererScreenHeight);

	std::shared_ptr<GUIWindowModel> model = std::make_shared<GUIWindowModelForGame>(p_renderer);
	std::shared_ptr<GUIWindowWinView> view = std::make_shared<GUIWindowWinView>();

	Keyboard keyboard;
	GameController gameController(p_renderer, &keyboard);

	GUIWindowControllerForGame ctrl(model, view, &gameController);
	if (ctrl.Start() == false)
		return -1;

	SAFETY_MEM_RELEASE(p_renderer);
#endif

	return 0;
}