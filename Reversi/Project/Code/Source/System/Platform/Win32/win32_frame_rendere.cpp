#include "System/Platform/Win32/win32_frame_renderer.h"

#include "common.h"

#include "Window/Code/Header/Render/PathTracing/raytrace_space.h"

Win32FrameRenderer::Win32FrameRenderer(const int in_w, const int in_h)
	: RenderingInterface()
{
	this->_draws.clear();

	// レイトレースする縦横比を16:9にする
	this->_p_raytrace_space = new RayTraceSpace(in_w, 16, 9);
	this->_width = in_w;
	this->_height = in_h;
}

Win32FrameRenderer::~Win32FrameRenderer()
{
	SAFETY_MEM_RELEASE(this->_p_raytrace_space);
}

/// <summary>
/// 描画インターフェイスの登録.
/// </summary>
void Win32FrameRenderer::AddViewInterface(RenderViewInterface* in_pAddRender)
{
	this->_draws.push_back(in_pAddRender);
}

/// <summary>
/// 描画インターフェイスの解除.
/// </summary>
void Win32FrameRenderer::RemoveViewInterface(RenderViewInterface* in_pRemoveRender)
{
	for (auto iter = this->_draws.begin(); iter != this->_draws.end(); ++iter)
	{
		if (*iter == in_pRemoveRender)
		{
			this->_draws.erase(iter);
			return;
		}
	}
}

// レンダーのテキスト縦横サイズ
const unsigned Win32FrameRenderer::GetTextRenderWidth() {
	return this->_width;
}

const unsigned Win32FrameRenderer::GetTextRenderHeight() {
	return this->_height;
}

// 画面クリア文字コード
const char Win32FrameRenderer::GetClsCharacteCode()
{
	return 0;
}

/// <summary>
/// 描画する半角文字を反映.
/// </summary>
void Win32FrameRenderer::FlashRectHalfCharacter(
	const char* in_pRectCharcterHalfSize,
	const int in_startPointX,
	const int in_startPointY,
	const unsigned int in_width,
	const unsigned int in_height)
{
	// TODO: テキスト描画する文字をテキストバッファを追加
}

/// <summary>
/// 描画する半角文字を1行単位で反映.
/// </summary>
void Win32FrameRenderer::FlashLineHalfCharacter(
	const char* in_pRectCharcterHalfSize,
	const int in_startPointX,
	const int in_startPointY)
{
	// TODO: テキスト描画する文字をテキストバッファを追加
}

/// <summary>
/// 描画.
/// </summary>
void Win32FrameRenderer::Draw()
{
	for (auto render : this->_draws)
		render->Draw(this);
}

/// <summary>
/// 描画遅延.
/// </summary>
void Win32FrameRenderer::DrawAfter()
{
}