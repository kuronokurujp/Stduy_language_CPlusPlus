#ifndef __WIN32_FRAME_RENDERER_H__
#define __WIN32_FRAME_RENDERER_H__

#include "System/Interface/console_render_interface.h"

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
	// 描画するテキスト文字列取得
	inline const std::string& GetRenderTextString() const { return this->_render_text_string; }

	// 画面クリア文字コード
	// TODO: 内部で利用する機会が今はない
	const char GetClsCharacteCode() override final { return 0; }

	/// <summary>
	/// 描画する半角文字を反映.
	/// </summary>
	void FlashRectHalfCharacter(
		const char* in_ppRectHalfCharcter,
		const int in_startPointX,
		const int in_startPointY,
		const unsigned int in_width,
		const unsigned int in_height) override final;

	/// <summary>
	/// 描画する半角文字を1行単位で反映.
	/// </summary>
	void FlashLineHalfCharacter(
		const char* in_pRectHalfCharcter,
		const int in_startPointX,
		const int in_startPointY) override final;

	/// <summary>
	/// 描画.
	/// </summary>
	void Draw() override final;

private:
	// 書き込まれた文字列をstringで出力
	bool _CreateScreenTextString();
	// 画面描画テキスト文字列の中身をクリア
	void _CleanCharacterMap();

	enum eHalfCharacterMapSize
	{
		eHalfCharacterMapSize_Width = 100,
		eHalfCharacterMapSize_Height = 34,
	};

	RayTraceSpace* _p_raytrace_space;
	int _width, _height;

	std::vector<RenderViewInterface*> _draws;
	std::string _render_text_string;

	// 画面に表示するテキスト文字列
	char _renderingHalfCharcterMap[eHalfCharacterMapSize_Height][(eHalfCharacterMapSize_Width + 1)];
};

#endif __WIN32_FRAME_RENDERER_H__
