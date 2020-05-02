#ifndef __TERMINAL_RENDER_COMPONENT_H__
#define __TERMINAL_RENDER_COMPONENT_H__

#include "Component/renderComponent.h"

class TerminalRenderComponent final : public RenderComponent
{
public:
	TerminalRenderComponent(Actor* in_pActor, ConsoleRenderingInterface* in_pRendering);
	~TerminalRenderComponent();

	/// <summary>
	/// 画面クリア.
	/// </summary>
	void Cls();

	/// <summary>
	/// テキストスクロール.
	/// </summary>
	void ScrollLineText();

	/// <summary>
	/// 1行テキストを書き込む.
	/// </summary>
	bool WriteLineText(const unsigned int in_lineNumber, const char* in_pWriteText);

	/// <summary>
	/// 矩形描画.
	/// </summary>
	/// <returns></returns>
	void Draw(ConsoleRenderingInterface* in_pRendering) override final;

private:
	void _Clear();

	unsigned int _textRectHeight;
	unsigned int _textRectWidth;
	unsigned int _textRectHalfCharactersMemSize;

	char _renderClsCharacterCode;
	char* _pDstBufferDrawRectHalfCharacter;
};

#endif // __TERMINAL_RENDER_COMPONENT_H__
