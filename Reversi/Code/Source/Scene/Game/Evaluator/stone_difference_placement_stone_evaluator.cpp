#include "Scene/Game/Evaluator/stone_difference_placement_stone_evaluator.h"

#include "Scene/Game/Actor/user_actor.h"

// 勝ち負けのそれぞれの評価値
enum  eVictoryEvaluator
{
	eVictoryEvaluator_Win = 1,
	eVictoryEvaluator_Lost = -1,
	eVictoryEvaluator_Draw = 0,
};

StoneDifferencePlacementStoneEvaluator::StoneDifferencePlacementStoneEvaluator()
{
	this->_Clear();
}

const int StoneDifferencePlacementStoneEvaluator::Evaluator(
	BoardStatusInteface* in_pBoardStatus,
	UserActor* in_pCurrentUserActor)
{
	int stoneDiff = in_pBoardStatus->GetPlaceStoneCount(BoardData::eStone_ColorBlack);
	stoneDiff -= in_pBoardStatus->GetPlaceStoneCount(BoardData::eStone_ColorWhite);

	// 白の手番で白が大きい場合は石数の差が-になるが白石の値は-1なので掛けて+になる
	int diff = in_pCurrentUserActor->GetUseStone() * stoneDiff;

	switch (this->_mode)
	{
	case eEvaluatorMode_Difference:
	{
		return diff;
	}
	case eEvaluatorMode_WinOrLostOrDraw:
	{
		if (diff > 0) return eVictoryEvaluator_Win;
		if (diff < 0) return eVictoryEvaluator_Lost;

		return eVictoryEvaluator_Draw;
	}
	}

	return 0;
}

void StoneDifferencePlacementStoneEvaluator::_Clear()
{
	this->_mode = eEvaluatorMode_Difference;
}