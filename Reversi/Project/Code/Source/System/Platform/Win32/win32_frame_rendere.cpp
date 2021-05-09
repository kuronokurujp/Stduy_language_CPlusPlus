#include "System/Platform/Win32/win32_frame_renderer.h"

#include <codecvt>

#include "common.h"

#include "Window/Code/Header/Render/PathTracing/raytrace_space.h"

Win32FrameRenderer::Win32FrameRenderer(const int in_w, const int in_h)
	: RenderingInterface()
{
	this->_CleanCharacterMap();
	// あらかじめ文字列のメモリ確保
	{
		this->_render_text_string.reserve(eHalfCharacterMapSize_Height * eHalfCharacterMapSize_Width);
		this->_old_render_text_string.reserve(eHalfCharacterMapSize_Height * eHalfCharacterMapSize_Width);
	}

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

/// <summary>
/// 描画する半角文字を反映.
/// </summary>
void Win32FrameRenderer::FlashRectHalfCharacter(
	const char* in_ppRectHalfCharcter,
	const int in_startPointX,
	const int in_startPointY,
	const unsigned int in_width,
	const unsigned int in_height)
{
	// 一行ずつ書き込む
	int mapY = 0;
	int mapX = 0;

	for (unsigned int y = 0; y < in_height; ++y)
	{
		mapY = y + in_startPointY;
		// 画面範囲外かチェック
		if ((mapY < 0) || (mapY >= eHalfCharacterMapSize_Height))
			continue;

		for (unsigned int x = 0; x < in_width; ++x)
		{
			mapX = x + in_startPointX;

			// 画面範囲外かチェック
			if ((mapX < 0) || (mapX >= eHalfCharacterMapSize_Width))
				continue;

			// 一文字ずつ書き込む
			this->_renderingHalfCharcterMap[mapY][mapX] = in_ppRectHalfCharcter[y * in_width + x];
		}
	}
}

/// <summary>
/// 描画する半角文字を1行単位で反映.
/// </summary>
void Win32FrameRenderer::FlashLineHalfCharacter(
	const char* in_pRectHalfCharcter,
	const int in_startPointX,
	const int in_startPointY)
{
	// TODO: テキスト描画する文字をテキストバッファを追加
	int mapY = in_startPointY;

	// 文字書き込み範囲外かチェック
	if ((mapY < 0) || (mapY >= eHalfCharacterMapSize_Height))
		return;

	unsigned int width = static_cast<unsigned int>(strlen(in_pRectHalfCharcter));
	int mapX = 0;
	for (unsigned int x = 0; x < width; ++x)
	{
		mapX = x + in_startPointX;

		// 文字書き込み範囲外かチェック
		if ((mapX < 0) || (mapX >= eHalfCharacterMapSize_Width))
			continue;

		// 一文字ずつ書き込む
		this->_renderingHalfCharcterMap[mapY][mapX] = in_pRectHalfCharcter[x];
	}
}

/// <summary>
/// 描画.
/// </summary>
void Win32FrameRenderer::Draw()
{
	for (auto render : this->_draws)
		render->Draw(this);

	// スクリーンに描画するテキスト文字列を作成
	this->_CreateScreenTextString();
	this->_CleanCharacterMap();
}

// 書き込まれた文字列をstringで出力
bool Win32FrameRenderer::_CreateScreenTextString()
{
	// 変更前の文字列をコピー
	this->_old_render_text_string = this->_render_text_string;

	// 書き込んだ文字列を整理して文字列型でまとめる
	// 文字はアスキーコード前提
	this->_render_text_string.clear();
	{
		for (unsigned int y = 0; y < eHalfCharacterMapSize_Height; ++y)
		{
			for (unsigned int x = 0; x < eHalfCharacterMapSize_Width; ++x)
				this->_render_text_string += this->_renderingHalfCharcterMap[y][x];

			// 改行コードを入れる
			if (y < (eHalfCharacterMapSize_Height - 1))
				this->_render_text_string += '\n';
		}
	}

	// 文字列があれば出力成功
	return (0 < this->_render_text_string.length());
}

// 画面描画テキスト文字列の中身をクリア
void Win32FrameRenderer::_CleanCharacterMap()
{
	// 0でうめると文字が続いているの途中で切られる
	::memset(this->_renderingHalfCharcterMap, ' ', sizeof(this->_renderingHalfCharcterMap));
}