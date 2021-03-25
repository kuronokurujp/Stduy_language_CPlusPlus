#ifndef __AI_ALPHABETA_COMPONENT_H__
#define __AI_ALPHABETA_COMPONENT_H__

#include "Component/component.h"
#include "Scene/Game/Interface/board_interface.h"
#include "Scene/Game/Interface/ai_component_interface.h"

// 前方宣言
class UserActor;
class PlacementStoneEvaluatorInterface;
class LearnBoardPlacementStoneEvaluator;
class StoneDifferencePlacementStoneEvaluator;

/// <summary>
/// MiniMaxアルゴリズムの「AlphaBeta」による打ち手.
/// </summary>
class AIAlphaBetaComponent final : public Component, public AIComponentInterface
{
public:
	// 評価値計算のロジックは別設定するべき
	AIAlphaBetaComponent(
		UserActor* in_pOwner,
		UserActor* in_pEnemyActor,
		BoardControllerInteface* in_pBoardInterface,
		BoardStatusInteface* in_pBoardStatus);
	~AIAlphaBetaComponent();

	/// <summary>
	/// 毎フレーム更新.
	/// </summary>
	void Update(const float in_deltaTimeSecond) override final;

	void Start() override final;

	// 現在の状態
	const eState GetState() const override final { return this->_state; }

	// AIが打った手の位置
	const BoardData::sPoint& GetPlacementPoint() const { return this->_placementPoint; }

private:
	struct StoneMove
	{
	public:
		StoneMove()
		{
			this->_point._x = this->_point._y = 0;
			this->_eval = 0;
		}

		StoneMove(const int in_x, const int in_y, const int in_eval)
		{
			this->_point._x = in_x;
			this->_point._y = in_y;
			this->_eval = in_eval;
		}

		BoardData::sPoint _point;
		int _eval;
	};

	// 石を打つのが終わった
	void _EndPlacementStone(const BoardData::sPoint& in_rPlacementPoint, const eState in_state);

	// 石が打てる石位置のリストを出力
	void _OutputHighScorePlacementStonePointList(
		std::vector<BoardData::sPoint>& out_rPlacementPoints,
		const std::vector<BoardData::sPoint>& in_rNowPlacementPoints,
		int in_limit,
		UserActor* in_pActiveUserActor,
		UserActor* in_pEnemyUserActor);

	/// <summary>
	/// AlphaBetaアルゴリズムによる探索.
	/// </summary>
	const int _SearchAlphabeta(
		int in_limite,
		int in_alpha,
		int in_beta,
		UserActor* in_pActiveUserActor,
		UserActor* in_pEnemyUserActor);

	void _Clear();

	eState _state;

	bool _startFlag, _placementStoneFlag;
	unsigned int _presearchDepth, _normalDepth;
	unsigned int _changeEvaluatorStoneCount;

	BoardData::sPoint _placementPoint;

	UserActor* _pSelfUserActor;
	UserActor* _pEnemyUserActor;
	BoardControllerInteface* _pBoardInterface;
	BoardStatusInteface* _pBoardStatus;

	// 現在使用する評価計算
	PlacementStoneEvaluatorInterface* _pUseEvaluator;

	// 盤の学習した評価計算
	LearnBoardPlacementStoneEvaluator* _pLearnBoardEvaluator;

	// 石の差による評価計算
	StoneDifferencePlacementStoneEvaluator* _pStoneDiffEvaluator;
};

#endif // __AI_ALPHABETA_COMPONENT_H__
