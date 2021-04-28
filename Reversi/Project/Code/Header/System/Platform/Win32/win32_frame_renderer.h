#ifndef __WIN32_FRAME_RENDERER_H__
#define __WIN32_FRAME_RENDERER_H__

#include "System/Interface/console_render_interface.h"

#include "Render/PathTracing/raytrace_space.h"

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

	RayTraceSpace* GetRayTraceSpacePtr() const { return this->_p_raytrace_space; }

	// 画面クリア文字コード
	const char GetClsCharacteCode() override final;

	/// <summary>
	/// 描画する半角文字を反映.
	/// </summary>
	void FlashRectHalfCharacter(
		const char* in_pRectCharcterHalfSize,
		const int in_startPointX,
		const int in_startPointY,
		const unsigned int in_width,
		const unsigned int in_height) override final;

	/// <summary>
	/// 描画する半角文字を1行単位で反映.
	/// </summary>
	void FlashLineHalfCharacter(
		const char* in_pRectCharcterHalfSize,
		const int in_startPointX,
		const int in_startPointY) override final;

	/// <summary>
	/// 描画.
	/// </summary>
	void Draw() override final;

private:
	RayTraceSpace* _p_raytrace_space;
	int _width, _height;

	std::vector<RenderViewInterface*> _draws;
};

#endif __WIN32_FRAME_RENDERER_H__
