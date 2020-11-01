#ifndef __STONE_HAND_COMPONENT_H__
#define __STONE_HAND_COMPONENT_H__

#include "Component/component.h"
#include "Scene/Game/Interface/board_Interface.h"

#include <string>

class StoneHandComponent : public Component
{
public:
	enum eState
	{
		eState_Start = 0,
		eState_Update,
		eState_End,
	};

	enum eResultPlacementStone
	{
		eResultPlacementStone_Sucess = 0,
		eResultPlacementStone_InputCountMiss,
		eResultPlacementStone_InputTextMiss,
		eResultPlacementStone_FlipMiss,
		eResultPlacementStone_Miss,
	};

	StoneHandComponent(Actor* in_pActor, const BoardData::eStone in_useStone, const char in_renderClsCharcterCode);

	void Reset(BoardControllerInteface* in_pBoardController, BoardStatusInteface* in_pBoardStatus);
	void Start();
	void End();

	// 未使用
	void Update(const float in_deltaTimeSecond) override final {}

	const eState GetState() const { return this->_state; }

	/// <summary>
	/// 位置テキストから石を設定.
	/// </summary>
	const eResultPlacementStone SetPlacementStone(const char* in_pStonePositionText);

	/// <summary>
	/// 盤の位置から石を設定.
	/// </summary>
	const eResultPlacementStone SetPlacementStone(const BoardData::sPoint& in_rBoardPoint);

	// 石を置いたコマンドのundo
	bool UndoPlacement(BoardData::sPoint& out_rUndoPoint);

	/// <summary>
	/// オブジェクトのテキスト設定.
	/// printfの書式指定出来る.
	/// </summary>
	void SetObjectText(const char* in_pFormat, ...);

	/// <summary>
	/// オブジェクトのテキスト化.
	/// </summary>
	const char* ToText();

	/// <summary>
	/// 使用する石.
	/// </bsummary>
	const BoardData::eStone GetUseStone() { return this->_stone; }

	/// <summary>
	/// 盤のステータスインスタンスを取得.
	/// </summary>
	/// <returns></returns>
	const BoardStatusInteface* GetBoardStatus() { return this->_pBoardStatus; }

private:
	void _Clear();

	eState _state;
	std::string _objectText;
	unsigned char _renderClsChacterCode;
	BoardControllerInteface* _pBoardController;
	BoardStatusInteface* _pBoardStatus;
	BoardData::eStone _stone;
};

#endif // __STONE_HAND_COMPONENT_H__
