#ifndef __WIN32_FRAME_RENDERER_H__
#define __WIN32_FRAME_RENDERER_H__

#include "common.h"
#include "System/Interface/console_render_interface.h"
#include "System/text_font.h"

#include "Window/Code/Header/Render/PathTracing/raytrace_space.h"

// レイトレースを利用した描画にする
class Win32FrameRenderer : public RenderingInterface
{
public:
	Win32FrameRenderer(const int in_w, const int in_h);
	virtual ~Win32FrameRenderer();

	/// <summary>
	/// 描画インターフェイスの登録.
	/// </summary>
	void AddViewInterface(RenderViewInterface* in_pAddRender) override final;

	/// <summary>
	/// 描画インターフェイスの解除.
	/// </summary>
	void RemoveViewInterface(RenderViewInterface* in_pRemoveRender) override final;

	// レンダーのテキスト縦横サイズ
	const unsigned GetTextRenderWidth() override final;
	const unsigned GetTextRenderHeight() override final;

	// レイトレース空間を扱うインスタンスポイントを取得
	inline RayTraceSpace* GetRayTraceSpacePtr() const { return this->_p_raytrace_space; }

	// 表示するテキストフォントリスト
	inline const TextFont* GetTextFontPtr(const unsigned in_index) const {
		if (StaticSingleArrayLength(this->_text_font_array) <= in_index)
			return NULL;

		return &this->_text_font_array[in_index];
	}

	// 画面クリア文字コード
	// TODO: 内部で利用する機会が今はない
	const char GetClsCharacteCode() override final { return 0; }

	/// <summary>
	/// 描画する半角文字を反映.
	/// </summary>
	void FlashRectHalfCharacter(
		const char* in_ppRectHalfCharcter,
		const float in_startPointX,
		const float in_startPointY,
		const unsigned int in_width,
		const unsigned int in_height) override final;

	/// <summary>
	/// 描画する半角文字を1行単位で反映.
	/// </summary>
	void FlashLineHalfCharacter(
		const char* in_pRectHalfCharcter,
		const float in_startPointX,
		const float in_startPointY) override final;

	/// <summary>
	/// 描画.
	/// </summary>
	void Draw() override final;

	void EndDraw() override final;

private:
	void _Clear()
	{
		this->_text_font_work_index = 0;
		this->_p_raytrace_space = nullptr;
		this->_width = this->_height = 0;
	}

	// 書き込まれた文字列をstringで出力
	//bool _CreateScreenTextString();
	// 画面描画テキスト文字列の中身をクリア
	void _CleanCharacterMap();

	RayTraceSpace* _p_raytrace_space;
	int _width, _height;

	std::vector<RenderViewInterface*> _draws;
	std::string _render_text_string;
	std::string _old_render_text_string;

	// 画面に表示するテキスト文字列
	TextFont _text_font_array[32];
	unsigned int _text_font_work_index;
};

#endif __WIN32_FRAME_RENDERER_H__
