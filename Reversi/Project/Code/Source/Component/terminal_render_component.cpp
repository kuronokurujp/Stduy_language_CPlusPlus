#include "Component/terminal_render_component.h"

#include "common.h"

TerminalRenderComponent::TerminalRenderComponent(Actor* in_pActor, RenderingInterface* in_pRendering)
	: RenderComponent(in_pActor, in_pRendering)
{
	this->_Clear();

	this->_renderClsCharacterCode = in_pRendering->GetClsCharacteCode();

	this->_textRectHalfCharactersMemSize = 0;
	// 描画するデータ作成
	{
		this->_text_rect_width = in_pRendering->GetTextRenderWidth();
		this->_text_rect_height = 10;

		// 描画メモリを確保
		this->_textRectHalfCharactersMemSize = this->_text_rect_width * this->_text_rect_height;
		// 描画バッファを用意
		this->_p_dst_buffer_draw_rect_half_character = reinterpret_cast<char*>(new char[this->_textRectHalfCharactersMemSize]);
		memset(this->_p_dst_buffer_draw_rect_half_character, this->_renderClsCharacterCode, this->_textRectHalfCharactersMemSize);
	}
}

TerminalRenderComponent::~TerminalRenderComponent()
{
	SAFETY_MEM_RELEASE(this->_p_dst_buffer_draw_rect_half_character);
}

/// <summary>
/// 画面クリア.
/// </summary>
void TerminalRenderComponent::Cls()
{
	memset(this->_p_dst_buffer_draw_rect_half_character, this->_renderClsCharacterCode, this->_textRectHalfCharactersMemSize);
}

/// <summary>
/// テキストスクロール.
/// </summary>
void TerminalRenderComponent::ScrollLineText()
{
	// 一行下にずらす
	char* pWriteTextPointer = &this->_p_dst_buffer_draw_rect_half_character[this->_text_rect_width * 1];
	unsigned lineMemSize = this->_textRectHalfCharactersMemSize / this->_text_rect_height;
	memcpy(pWriteTextPointer, this->_p_dst_buffer_draw_rect_half_character, this->_textRectHalfCharactersMemSize - lineMemSize);
}

/// <summary>
/// 1行テキストを書き込む.
/// </summary>
bool TerminalRenderComponent::WriteLineText(const unsigned int in_linePosition, const char* in_pWriteText)
{
	if (this->_text_rect_height <= in_linePosition)
	{
		return false;
	}

	char* pWriteTextPointer = &this->_p_dst_buffer_draw_rect_half_character[this->_text_rect_width * in_linePosition];
	unsigned lineMemSize = this->_textRectHalfCharactersMemSize / this->_text_rect_height;
	// 描画ラインのクリア
	memset(pWriteTextPointer, this->_renderClsCharacterCode, lineMemSize);

	int srcMemSize = static_cast<int>(strlen(in_pWriteText)) * sizeof(in_pWriteText[0]);
	memcpy_s(pWriteTextPointer, lineMemSize, in_pWriteText, srcMemSize);

	return true;
}

/// <summary>
/// 矩形描画.
/// </summary>
/// <returns></returns>
void TerminalRenderComponent::Draw(RenderingInterface* in_pRendering)
{
	// 描画バッファに転送
	// TODO: GUI用のテキスト設定が必要
#ifdef __CUI_GAME__
	in_pRendering->FlashRectHalfCharacter(
		this->_p_dst_buffer_draw_rect_half_character,
		0,
		20,
		this->_text_rect_width,
		this->_text_rect_height
	);
#endif
}

void TerminalRenderComponent::_Clear()
{
	this->_text_rect_height = 0;
	this->_text_rect_width = 0;
	this->_textRectHalfCharactersMemSize = 0;
	this->_renderClsCharacterCode = 0;

	this->_p_dst_buffer_draw_rect_half_character = NULL;
}