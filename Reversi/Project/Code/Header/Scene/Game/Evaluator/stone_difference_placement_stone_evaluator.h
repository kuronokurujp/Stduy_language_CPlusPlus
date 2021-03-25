#ifndef __STONE_DIFFERENCE_PLACEMENT_STONE_EVALUATOR_H__
#define __STONE_DIFFERENCE_PLACEMENT_STONE_EVALUATOR_H__

#include "Scene/Game/Interface/placement_stone_evaluator_interface.h"

/// <summary>
/// 石の個数による評価.
/// </summary>
/// <seealso cref="PlacementStoneEvaluatorInterface" />
class StoneDifferencePlacementStoneEvaluator : public PlacementStoneEvaluatorInterface
{
public:
	enum eEvaluatorMode
	{
		eEvaluatorMode_Difference = 0,
		eEvaluatorMode_WinOrLostOrDraw,
	};

	StoneDifferencePlacementStoneEvaluator();

	void SetMode(const eEvaluatorMode in_mode) { this->_mode = in_mode; }

	const int Evaluator(BoardStatusInteface* in_pBoardStatus, UserActor* in_pCurrentUserActor);

private:
	void _Clear();

	eEvaluatorMode _mode;
};

#endif // __STONE_DIFFERENCE_PLACEMENT_STONE_EVALUATOR_H__
