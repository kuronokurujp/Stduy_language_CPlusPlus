#include "Scene/Game/Component/stonehand_component.h"

#include <cstdarg>
#include <cctype>
#include <stdio.h>

// 文字数最大
static const unsigned int s_objectTextLength = 256;

StoneHandComponent::StoneHandComponent(Actor* in_pActor, const BoardData::eStone in_useStone, const char in_renderClsCharcterCode)
	: Component(in_pActor)
{
	this->_Clear();

	this->_stone = in_useStone;
	this->_renderClsChacterCode = in_renderClsCharcterCode;

	this->_objectText.reserve(s_objectTextLength);
}

void StoneHandComponent::Reset(BoardControllerInteface* in_pBoardController, BoardStatusInteface* in_pBoardStatus)
{
	this->_state = eState_Start;
	this->_pBoardController = in_pBoardController;
	this->_pBoardStatus = in_pBoardStatus;
}

void StoneHandComponent::Start()
{
	this->_state = eState_Update;

	// 石が置けないならパス
	if (this->_pBoardStatus->IsPlacementStone(this->_stone) == false)
	{
		// 石が置けないので終了
		this->End();
	}

	// 打ち手が打てる箇所にマークを付ける
	this->_pBoardStatus->InputPlaceStoneMark(this->_stone);
}

void StoneHandComponent::End()
{
	this->_state = eState_End;
	// TODO: マスにつけたマークをクリア
	this->_pBoardStatus->ClearPlaceStoneMark();
}

/// <summary>
/// 位置テキストから石を設定.
/// </summary>
const StoneHandComponent::eResultPlacementStone
StoneHandComponent::SetPlacementStone(const char* in_pStonePositionText)
{
	// 文字数が2文字かチェック
	if (strlen(in_pStonePositionText) != 2)
	{
		return eResultPlacementStone_InputCountMiss;
	}

	// 片方の文字がアルファベット、もう片方が数字かチェック
	char alpha = 0;
	unsigned int number = 0;
	{
		if (isalpha(in_pStonePositionText[0]) && isdigit(in_pStonePositionText[1]))
		{
			alpha = in_pStonePositionText[0];
			number = atoi(&in_pStonePositionText[1]);
		}
		else if (isalpha(in_pStonePositionText[1]) && isdigit(in_pStonePositionText[0]))
		{
			alpha = in_pStonePositionText[1];
			number = atoi(&in_pStonePositionText[0]);
		}
		else
		{
			return eResultPlacementStone_InputTextMiss;
		}
	}

	// 石を置く
	{
		BoardControllerInteface::eResultCommand result = this->_pBoardController->CommandPlacementStone(alpha, number, this->_stone);
		switch (result)
		{
		case BoardControllerInteface::eResultCommand_PlacementStoneMiss:
			return eResultPlacementStone_Miss;

		case BoardControllerInteface::eResultCommand_FlipStoneMiss:
			return eResultPlacementStone_FlipMiss;
		}
	}

	return eResultPlacementStone_Sucess;
}

/// <summary>
/// 盤の位置から石を設定.
/// </summary>
const StoneHandComponent::eResultPlacementStone
StoneHandComponent::SetPlacementStone(const BoardData::sPoint& in_rBoardPoint)
{
	// 石を置く
	{
		BoardControllerInteface::eResultCommand result = this->_pBoardController->CommandPlacementStone(in_rBoardPoint, this->_stone);
		switch (result)
		{
		case BoardControllerInteface::eResultCommand_PlacementStoneMiss:
			return eResultPlacementStone_Miss;

		case BoardControllerInteface::eResultCommand_FlipStoneMiss:
			return eResultPlacementStone_FlipMiss;
		}
	}

	return eResultPlacementStone_Sucess;
}

// 石を置いたコマンドのundo
bool StoneHandComponent::UndoPlacement(BoardData::sPoint& out_rUndoPoint)
{
	return this->_pBoardController->CommandUndoPlacement(this->_stone);
}

/// <summary>
/// オブジェクトのテキスト設定.
/// </summary>
void StoneHandComponent::SetObjectText(const char* in_pFormat, ...)
{
	std::va_list vaList;
	va_start(vaList, in_pFormat);

	char aCharacterBuffer[s_objectTextLength];
	memset(aCharacterBuffer, this->_renderClsChacterCode, sizeof(aCharacterBuffer));
	std::vsnprintf(aCharacterBuffer, sizeof(aCharacterBuffer), in_pFormat, vaList);

	va_end(vaList);

	this->_objectText = aCharacterBuffer;
}

/// <summary>
/// オブジェクトのテキスト化.
/// </summary>
const char* StoneHandComponent::ToText()
{
	return this->_objectText.c_str();
}

void StoneHandComponent::_Clear()
{
	this->_stone = BoardData::eStone_None;
	this->_state = eState_Start;
	this->_objectText.clear();
	this->_renderClsChacterCode = 0;
	this->_pBoardController = NULL;
	this->_pBoardStatus = NULL;
}