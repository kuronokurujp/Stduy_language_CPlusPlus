#include "Component/terminalRenderComponent.h"

#include "common.h"

TerminalRenderComponent::TerminalRenderComponent(Actor* in_pActor, ConsoleRenderingInterface* in_pRendering)
	: RenderComponent(in_pActor, in_pRendering)
{
	this->_Clear();

	this->_renderClsCharacterCode = in_pRendering->GetClsCharacteCode();

	this->_textRectHalfCharactersMemSize = 0;
	// 描画するデータ作成
	{
		this->_textRectWidth = in_pRendering->GetTextRenderWidth();
		this->_textRectHeight = 10;

		// 描画メモリを確保
		this->_textRectHalfCharactersMemSize = this->_textRectWidth * this->_textRectHeight;
		// 描画バッファを用意
		this->_pDstBufferDrawRectHalfCharacter = reinterpret_cast<char*>(new char[this->_textRectHalfCharactersMemSize]);
		memset(this->_pDstBufferDrawRectHalfCharacter, this->_renderClsCharacterCode, this->_textRectHalfCharactersMemSize);
	}
}

TerminalRenderComponent::~TerminalRenderComponent()
{
	SAFETY_MEM_RELEASE(this->_pDstBufferDrawRectHalfCharacter);
}

/// <summary>
/// 画面クリア.
/// </summary>
void TerminalRenderComponent::Cls()
{
	memset(this->_pDstBufferDrawRectHalfCharacter, this->_renderClsCharacterCode, this->_textRectHalfCharactersMemSize);
}

/// <summary>
/// テキストスクロール.
/// </summary>
void TerminalRenderComponent::ScrollLineText()
{
	// 一行下にずらす
	char* pWriteTextPointer = &this->_pDstBufferDrawRectHalfCharacter[this->_textRectWidth * 1];
	unsigned lineMemSize = this->_textRectHalfCharactersMemSize / this->_textRectHeight;
	memcpy(pWriteTextPointer, this->_pDstBufferDrawRectHalfCharacter, this->_textRectHalfCharactersMemSize - lineMemSize);
}

/// <summary>
/// 1行テキストを書き込む.
/// </summary>
bool TerminalRenderComponent::WriteLineText(const unsigned int in_linePosition, const char* in_pWriteText)
{
	if (this->_textRectHeight <= in_linePosition)
	{
		return false;
	}

	char* pWriteTextPointer = &this->_pDstBufferDrawRectHalfCharacter[this->_textRectWidth * in_linePosition];
	unsigned lineMemSize = this->_textRectHalfCharactersMemSize / this->_textRectHeight;
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
void TerminalRenderComponent::Draw(ConsoleRenderingInterface* in_pRendering)
{
	// 描画バッファに転送
	in_pRendering->FlashRectHalfCharacter(
		this->_pDstBufferDrawRectHalfCharacter,
		0,
		20,
		this->_textRectWidth,
		this->_textRectHeight
	);
}

void TerminalRenderComponent::_Clear()
{
	this->_textRectHeight = 0;
	this->_textRectWidth = 0;
	this->_textRectHalfCharactersMemSize = 0;
	this->_renderClsCharacterCode = 0;

	this->_pDstBufferDrawRectHalfCharacter = NULL;
}