#include "System/Platform/Win32/win32_frame_renderer.h"

#include <codecvt>

#include "common.h"

#include "Window/Code/Header/Render/PathTracing/raytrace_space.h"

Win32FrameRenderer::Win32FrameRenderer(const int in_w, const int in_h)
	: RenderingInterface()
{
	this->_Clear();

	this->_CleanCharacterMap();

	this->_draws.clear();

	// レイトレースする縦横比を16:9にする
	this->_p_raytrace_space = new RayTraceSpace(in_w, 16, 9);
	this->_width = in_w;
	this->_height = in_h;
}

Win32FrameRenderer::~Win32FrameRenderer() {
	SAFETY_MEM_RELEASE(this->_p_raytrace_space);
}

/// <summary>
/// 描画インターフェイスの登録.
/// </summary>
void Win32FrameRenderer::AddViewInterface(RenderViewInterface* in_pAddRender) {
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

/// <summary>
/// 描画する半角文字を反映.
/// </summary>
void Win32FrameRenderer::FlashRectHalfCharacter(
	const char* in_ppRectHalfCharcter,
	const float in_startPointX,
	const float in_startPointY,
	const unsigned int in_width,
	const unsigned int in_height)
{
	std::string text;
	for (unsigned int y = 0; y < in_height; ++y)
	{
		// 一文字をつなげて一行の文字列にする
		text.clear();
		for (unsigned int x = 0; x < in_width; ++x)
			// 一文字ずつ書き込む
			text += in_ppRectHalfCharcter[y * in_width + x];

		// 一文字なので末尾に改行
		text += "\n";
		// 文字列を一行書き込む
		auto offset_y = static_cast<float>(y) / static_cast<float>(in_height);
		offset_y *= 0.7f;
		this->FlashLineHalfCharacter(text.c_str(), in_startPointX, in_startPointY + offset_y);
	}
}

/// <summary>
/// 描画する半角文字を1行単位で反映.
/// </summary>
void Win32FrameRenderer::FlashLineHalfCharacter(
	const char* in_pRectHalfCharcter,
	const float in_startPointX,
	const float in_startPointY)
{
	assert(this->_text_font_work_index < StaticSingleArrayLength(this->_text_font_array));

	auto p_work_text_font = &this->_text_font_array[this->_text_font_work_index];

	strcpy_s(p_work_text_font->_text, StaticSingleArrayLength(p_work_text_font->_text), in_pRectHalfCharcter);
	p_work_text_font->_x = in_startPointX;
	p_work_text_font->_y = in_startPointY;

	++this->_text_font_work_index;
}

/// <summary>
/// 描画.
/// </summary>
void Win32FrameRenderer::Draw()
{
	for (auto render : this->_draws)
		render->Draw(this);
}

void Win32FrameRenderer::EndDraw()
{
	this->_CleanCharacterMap();
}

// 画面描画テキスト文字列の中身をクリア
void Win32FrameRenderer::_CleanCharacterMap()
{
	::memset(this->_text_font_array, 0, sizeof(this->_text_font_array));
	this->_text_font_work_index = 0;
}