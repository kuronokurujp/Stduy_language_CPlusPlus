#ifndef __TERMINAL_RENDER_COMPONENT_H__
#define __TERMINAL_RENDER_COMPONENT_H__

#include "Component/render_component.h"

class TerminalRenderComponent final : public RenderComponent
{
public:
	TerminalRenderComponent(Actor* in_pActor, RenderingInterface* in_pRendering);
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
	void Draw(RenderingInterface* in_pRendering) override final;

private:
	void _Clear();

	unsigned int _text_rect_height;
	unsigned int _text_rect_width;
	unsigned int _textRectHalfCharactersMemSize;

	char _renderClsCharacterCode;
	char* _p_dst_buffer_draw_rect_half_character;
};

#endif // __TERMINAL_RENDER_COMPONENT_H__
