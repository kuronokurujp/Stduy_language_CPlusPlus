#ifndef __PALCEMENT_STONE_EVALUATOR_INTERFACE_H__
#define __PALCEMENT_STONE_EVALUATOR_INTERFACE_H__

// 前方宣言
class BoardStatusInteface;
class UserActor;

class PlacementStoneEvaluatorInterface
{
public:
	virtual const int Evaluator(
		BoardStatusInteface* in_pBoardStatus, UserActor* in_pCurrentUserActor) = 0;
};

#endif // __PALCEMENT_STONE_EVALUATOR_INTERFACE_H__
