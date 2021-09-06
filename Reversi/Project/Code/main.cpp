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
		this->_Clear();

		this->_p_renderer = in_p_renderer;
		int width = this->_p_renderer->GetTextRenderWidth();
		int height = this->_p_renderer->GetTextRenderHeight();

		const double d_width = width - 1;
		const double d_height = height - 1;

		this->_inv_d_width = 1.0 / d_width;
		this->_inv_d_heigth = 1.0 / d_height;

		// フォントテキストデータを作成
		{
			const char* p_font_file_name = "Asset/Font/Roboto/Roboto-Black.ttf";

			this->_font_core_data = std::make_shared<SimpleFontCoreData>();
			this->_p_font_text_data = new SimpleFontTextData(
				this->_font_core_data, p_font_file_name
			);

			// あらかじめフォント画像を作成する
			{
				// 使用するフォント文字を文字列で記述
				// 英数字と記号限定
				const char* font_char_array = " 0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ`~!@#$%^&*()_+|}{""':=-\\][';/.,<>";
				this->_image_rect_buffer_max = strlen(font_char_array);

				// 一文字ずつ取得して画像を作成
				// 画像を1文字ずつ生成している
				this->_p_font_image_buffers = new _struct_text_font_image[this->_image_rect_buffer_max];
				char temp_char_array[2];
				::memset(temp_char_array, 0, sizeof(temp_char_array));

				for (unsigned int i = 0; i < this->_image_rect_buffer_max; ++i)
				{
					auto font_image = &this->_p_font_image_buffers[i];

					font_image->ascll_code = font_char_array[i];
					temp_char_array[0] = font_image->ascll_code;

					// テキスト画像を出力してバッファに保存
					this->_p_font_text_data->WriteFontImageTextRect(
						&font_image->text_image_rect,
						temp_char_array,
						16, 2);
				}
			}
		}
	}

	~GUIWindowModelForGame()
	{
		// フォントデータを破棄
		{
			// 生成したフォント画像を破棄
			// デストラクタでバッファを解放している
			SAFETY_MEM_ARRAY_RELEASE(this->_p_font_image_buffers);

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

		// テキストフォント描画
		{
			unsigned int text_font_index = 0;
			const TextFont* p_text_font = this->_p_renderer->GetTextFontPtr(text_font_index);

			// TextFontのデータが見つからなくなるまで続ける
			while (p_text_font != NULL)
			{
				// ゲーム側で設定したテキストデータを取得
				p_text_font = this->_p_renderer->GetTextFontPtr(text_font_index);
				// テキストがある場合は描画対応
				if (0 < strlen(p_text_font->_text))
				{
					// 正規化デバイス座標系(-1 - 1)からスクリーン座標系に直す
					auto pixel_begin_x = static_cast<unsigned int>(static_cast<float>(p_frame_buffer->Width()) * p_text_font->_x);
					auto pixel_begin_y = static_cast<unsigned int>(static_cast<float>(p_frame_buffer->Height()) * p_text_font->_y);

					for (unsigned int char_idx = 0; char_idx < strlen(p_text_font->_text); ++char_idx)
					{
						auto c = p_text_font->_text[char_idx];
						_struct_text_font_image* p_find_image = NULL;
						// 文字がフォント画像にあるか
						{
							for (unsigned int find_idx = 0; find_idx < this->_image_rect_buffer_max; ++find_idx)
							{
								auto image = &this->_p_font_image_buffers[find_idx];
								if (image->ascll_code == c)
								{
									p_find_image = image;
									break;
								}
							}
						}

						if (p_find_image != NULL)
						{
							// 画面に転送
							unsigned int p_w = p_find_image->text_image_rect.FontWidth();
							unsigned int w = __min(
								this->_p_renderer->GetTextRenderWidth(), p_w + pixel_begin_x);

							unsigned int p_h = p_find_image->text_image_rect.FontHeight();
							unsigned int h = __min(
								this->_p_renderer->GetTextRenderHeight(), p_h + pixel_begin_y);
							for (unsigned int y = pixel_begin_y, p_y = 0; (y < h) && (p_y < p_h); ++y, ++p_y)
							{
								for (unsigned int x = pixel_begin_x, p_x = 0; (x < w) && (p_x < p_w); ++x, ++p_x)
								{
									// 転送しないクリップする色ならスキップ
									// TODO: ふちが削れて見た目の質が悪い
									auto c = p_find_image->text_image_rect.GetBuffer(p_x, p_y).gray_scale;
									if (c == 0)
										continue;

									p_frame_buffer->SetPixel(x, y, c);
								}
							}

							// 表示位置を横にずらす
							pixel_begin_x += p_w;
						}
						// 表示した文字が画像フォントにない
						// エラー扱いにする
						else
							assert(0);
					}
				}

				++text_font_index;
				p_text_font = this->_p_renderer->GetTextFontPtr(text_font_index);
			};
		}

		GUIWindowModel::UpdateRender();
	}

	// タッチした3Dモデルをリスト出力
	bool OutputTouch3DModels(std::vector<hit_record>& out_r_list, const int in_x, const int in_y)
	{
		// TODO: レイを使って３Dモデルとヒットしているかの実装
		auto p_space = this->_p_renderer->GetRayTraceSpacePtr();

		Ray ray;
		// スクリーン情報から正規化デバイス座標系に変換
		auto v = double(in_y) * this->_inv_d_heigth;
		auto u = double(in_x) * this->_inv_d_width;

		p_space->_camera.OutputRay(&ray, u, v);

		hit_record rec;
		if (p_space->OutputRayHitRecord(&rec, ray)) {
			out_r_list.push_back(rec);
			return true;
		}

		return false;
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
	struct _struct_text_font_image
	{
		char ascll_code;
		SimpleTextImageRect text_image_rect;
	};

	void _Clear()
	{
		this->_p_renderer = nullptr;
		this->_inv_d_width = 0.0;
		this->_inv_d_heigth = 0.0;

		this->_p_font_text_data = nullptr;
		this->_p_font_image_buffers = NULL;
		this->_image_rect_buffer_max = 0;
	}

	Win32FrameRenderer* _p_renderer;
	double _inv_d_width;
	double _inv_d_heigth;

	std::shared_ptr<SimpleFontCoreData> _font_core_data;
	SimpleFontTextData* _p_font_text_data;
	_struct_text_font_image* _p_font_image_buffers;
	unsigned int _image_rect_buffer_max;
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

	void EndRender() override final
	{
		this->_p_game_ctrl->EndRender();
		GUIWindowController::EndRender();
	}

	// タッチイベント
	void OnTouchEvent(int in_type, const int in_x, const int in_y) override final
	{
		// マウスクリックを受け取り、レイトレースで描画しているモデルをタッチしているかチェック
		if (in_type == GUIWindowController::eTouchEvent::TOUCH_EVENT_L_CLICK)
		{
			auto model = std::dynamic_pointer_cast<GUIWindowModelForGame>(this->GetModel());

			// クリックしたモデル情報を出力
			std::vector<hit_record> hit_list;

			// タッチデータの情報リスト
			std::vector<InputComponent::_touch_event_data_> touch_list;
			if (model->OutputTouch3DModels(hit_list, in_x, in_y)) {
				InputComponent::_touch_event_data_ touch_event;
				for (auto it = hit_list.begin(); it != hit_list.end(); ++it)
				{
#if _DEBUG
					printf("s ------------------------------------------- \n");
					printf("hit! \n");
					printf("handle = %ul\n", it->object_handle);
					printf("u = %lf\n", it->u);
					printf("v = %lf\n", it->v);
					printf("e ------------------------------------------- \n");
#endif
					touch_event._touch_model.handle = it->object_handle;
					touch_list.push_back(touch_event);
			}
		}
			this->_p_game_ctrl->OnTouchEvent(
				InputComponent::eTouchEvent::TOUCH_EVENT_MODEL, touch_list);
	}
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