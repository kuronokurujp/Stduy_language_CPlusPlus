#ifndef __BOARD_RENDER_COMPONENT_H__
#define __BOARD_RENDER_COMPONENT_H__

#include "Component/render_component.h"
#include "Scene/Game/Interface/board_interface.h"

// 前方宣言
class BoardActor;

class BoardRenderComponent final : public RenderComponent
{
public:
	BoardRenderComponent(BoardActor* in_pBoard, RenderingInterface* in_pRendering);
	~BoardRenderComponent();

	/// <summary>
	/// 描画.
	/// </summary>
	/// <returns></returns>
	void Draw(RenderingInterface* in_pRendering) override final;

	/// <summary>
	/// 指定した石から文字コード取得.
	/// </summary>
	const char GetStoneCharacterCode(const BoardData::eStone in_stone);

	void SetStoneCount(const BoardData::eStone in_stone, const unsigned int in_count);

private:
	void _Clear();

	BoardActor* _pBoard;

	unsigned int _textRectHeight, _textRectWidth;
	unsigned int _blackStoneCount, _whiteStoneCount;

	char* _pDstBufferDrawRectHalfCharacter;
	char _renderClsCharacteCode;
};

#endif // __BOARD_RENDER_COMPONENT_H__