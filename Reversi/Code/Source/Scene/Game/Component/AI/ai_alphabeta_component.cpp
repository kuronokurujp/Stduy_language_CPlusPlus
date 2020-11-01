#include "Scene/Game/Component/AI/ai_alphabeta_component.h"

#include "common.h"

#include "Scene/Game/Actor/user_actor.h"

#include "Scene/Game/Interface/board_interface.h"

#include "Scene/Game/Evaluator/learn_borad_placement_stone_evaluator.h"
#include "Scene/Game/Evaluator/stone_difference_placement_stone_evaluator.h"

#include <algorithm>
#include <limits>

AIAlphaBetaComponent::AIAlphaBetaComponent(
	UserActor* in_pOwner,
	UserActor* in_pEnemyActor,
	BoardControllerInteface* in_pBoardInterface,
	BoardStatusInteface* in_pBoardStatus) : Component(in_pOwner)
{
	this->_Clear();

	this->_pSelfUserActor = in_pOwner;
	this->_pEnemyUserActor = in_pEnemyActor;
	this->_pBoardInterface = in_pBoardInterface;
	this->_pBoardStatus = in_pBoardStatus;

	// 探索する深さを設定
	// この値が大きい程、検索コストが高くなる
	{
		// 事前探索
		this->_presearchDepth = 3;

		// 通常探索
		this->_normalDepth = 2;
	}

	// 評価を切り替える石の数
	this->_changeEvaluatorStoneCount = this->_pBoardStatus->GetBoardSide() * this->_pBoardStatus->GetBoardSide();
	this->_changeEvaluatorStoneCount -= 10;

	this->_pLearnBoardEvaluator = new LearnBoardPlacementStoneEvaluator();
	this->_pStoneDiffEvaluator = new StoneDifferencePlacementStoneEvaluator();
}

AIAlphaBetaComponent::~AIAlphaBetaComponent()
{
	SAFETY_MEM_RELEASE(this->_pLearnBoardEvaluator);
	SAFETY_MEM_RELEASE(this->_pStoneDiffEvaluator);
}

/// <summary>
/// 毎フレーム更新.
/// </summary>
void AIAlphaBetaComponent::Update(const float in_deltaTimeSecond)
{
	if (this->_startFlag == false)
	{
		return;
	}

	// 打てる箇所をリストで取得
	const std::vector<BoardData::sPoint>& nowPlacmentStoneList = this->_pBoardStatus->GetPlacementStonePointList(this->_pSelfUserActor->GetUseStone());

	// 打てる箇所がなければスキップ
	if (nowPlacmentStoneList.empty())
	{
		this->_EndPlacementStone(BoardData::sPoint(0, 0), eState_SkipPlacement);
		return;
	}

	// 打てる箇所が一つしかないのであればそこに打つ
	if (nowPlacmentStoneList.size() == 1)
	{
		//		this->_pBoardInterface->CommandPlacementStone(nowPlacmentStoneList[0], this->_pSelfUserActor->GetUseStone());
		this->_EndPlacementStone(nowPlacmentStoneList[0], eState_SuccessPlacement);
		return;
	}

	// ソート用に一時評価計算を決める
	this->_pUseEvaluator = this->_pLearnBoardEvaluator;

	// 事前に打って高い評価リストを取得
	// 探索時間を軽減のため
	int limit = 0;

	std::vector<BoardData::sPoint> placmentStoneList;
	this->_OutputHighScorePlacementStonePointList(
		placmentStoneList,
		nowPlacmentStoneList,
		this->_presearchDepth,
		this->_pSelfUserActor,
		this->_pEnemyUserActor);

	// 石を置いた数に応じて探索の深さ数と評価方法を変える
	{
		unsigned int count = 0;

		count += this->_pBoardStatus->GetPlaceStoneCount(this->_pSelfUserActor->GetUseStone());
		count += this->_pBoardStatus->GetPlaceStoneCount(this->_pEnemyUserActor->GetUseStone());
		if (count >= this->_changeEvaluatorStoneCount)
		{
			// 置いた石の数に応じて勝ち負けか石の差のどちらかを評価にするか決める
			if (count >= (this->_changeEvaluatorStoneCount + 5))
			{
				this->_pStoneDiffEvaluator->SetMode(StoneDifferencePlacementStoneEvaluator::eEvaluatorMode_WinOrLostOrDraw);
			}
			else
			{
				this->_pStoneDiffEvaluator->SetMode(StoneDifferencePlacementStoneEvaluator::eEvaluatorMode_Difference);
			}

			this->_pUseEvaluator = this->_pStoneDiffEvaluator;

			limit = 5;
		}
		else
		{
			limit = this->_normalDepth;
		}
	}

	// 事前に受け取った評価が高い手のリストで打つ手を決める
	int alpha, beta;
	alpha = beta = -std::numeric_limits<int>::max();
	BoardData::sPoint p;

	int enemyGoodScore, enemeyBadScore = 0;

	for (unsigned int i = 0; i < placmentStoneList.size(); ++i)
	{
		this->_pBoardInterface->CommandPlacementStone(placmentStoneList[i], this->_pSelfUserActor->GetUseStone());

		// 相手の最善手のスコア取得
		enemyGoodScore = this->_SearchAlphabeta(limit - 1, -beta, -alpha, this->_pEnemyUserActor, this->_pSelfUserActor);

		this->_pBoardInterface->CommandUndoPlacement(this->_pSelfUserActor->GetUseStone());

		// -を付けて比較する事で相手の最善手の中でもっとも低いスコアを採用するようにする
		enemeyBadScore = -enemyGoodScore;
		if (enemeyBadScore >= alpha)
		{
			alpha = enemeyBadScore;
			p = placmentStoneList[i];
		}
	}

	//	this->_pBoardInterface->CommandPlacementStone(p, this->_pSelfUserActor->GetUseStone());
	this->_EndPlacementStone(p, eState_SuccessPlacement);
}

void AIAlphaBetaComponent::Start()
{
	this->_startFlag = true;
	this->_placementStoneFlag = false;
	this->_state = eState_Calculating;
}

void AIAlphaBetaComponent::_OutputHighScorePlacementStonePointList(
	std::vector<BoardData::sPoint>& out_rPlacementPoints,
	const std::vector<BoardData::sPoint>& in_rNowPlacementPoints,
	int in_limit,
	UserActor* in_pActiveUserActor,
	UserActor* in_pEnemyUserActor)
{
	std::vector<StoneMove> moves;
	int eval = 0;

	int alpha, beta;
	alpha = beta = std::numeric_limits<int>::max();
	alpha = -alpha;

	// 検証
	for (unsigned int i = 0; i < in_rNowPlacementPoints.size(); ++i)
	{
		this->_pBoardInterface->CommandPlacementStone(in_rNowPlacementPoints[i], in_pActiveUserActor->GetUseStone());
		eval = -this->_SearchAlphabeta(in_limit - 1, alpha, beta, in_pEnemyUserActor, in_pActiveUserActor);
		this->_pBoardInterface->CommandUndoPlacement(in_pActiveUserActor->GetUseStone());

		StoneMove move(in_rNowPlacementPoints[i]._x, in_rNowPlacementPoints[i]._y, eval);
		moves.push_back(move);
	}

	// 評価値に応じてソート
	std::stable_sort(
		moves.begin(),
		moves.end(),
		[](const StoneMove& in_rLhs, const StoneMove& in_rRhs) { return in_rLhs._eval > in_rRhs._eval; });

	// 出力
	out_rPlacementPoints.clear();
	for (unsigned int i = 0; i < moves.size(); ++i)
	{
		out_rPlacementPoints.push_back(BoardData::sPoint(moves[i]._point._x, moves[i]._point._y));
	}
}

const int AIAlphaBetaComponent::_SearchAlphabeta(
	int in_limite,
	int in_alpha,
	int in_beta,
	UserActor* in_pActiveUserActor,
	UserActor* in_pEnemyUserActor)
{
	const std::vector<BoardData::sPoint> placementPoints = this->_pBoardStatus->GetPlacementStonePointList(in_pActiveUserActor->GetUseStone());

	// 探索の末尾に到達したら評価計算をする
	// あるいはもう打てる手がない場合
	if (this->_pBoardStatus->IsCannotPlacementStone()
		|| (in_limite == 0)
		|| placementPoints.empty())
	{
		return this->_pUseEvaluator->Evaluator(this->_pBoardStatus, in_pActiveUserActor);
	}

	int score = 0;
	for (unsigned int i = 0; i < placementPoints.size(); ++i)
	{
		// 打つ
		this->_pBoardInterface->CommandPlacementStone(placementPoints[i], in_pActiveUserActor->GetUseStone());
		// 打った手から評価値を得る
		score = -this->_SearchAlphabeta(in_limite - 1, -in_beta, -in_alpha, in_pEnemyUserActor, in_pActiveUserActor);
		// 一旦打った手は戻す
		this->_pBoardInterface->CommandUndoPlacement(in_pActiveUserActor->GetUseStone());

		if (score > in_beta)
		{
			return score;
		}

		in_alpha = std::max(in_alpha, score);
	}

	return in_alpha;
}

void AIAlphaBetaComponent::_EndPlacementStone(const BoardData::sPoint& in_rPlacementPoint, const eState in_state)
{
	this->_startFlag = false;
	this->_placementStoneFlag = true;
	this->_placementPoint = in_rPlacementPoint;
	this->_state = in_state;
}

void AIAlphaBetaComponent::_Clear()
{
	this->_state = eState_None;

	this->_startFlag = false;
	this->_placementStoneFlag = false;
	this->_presearchDepth = this->_normalDepth = 0;
	this->_changeEvaluatorStoneCount = 0;

	this->_pSelfUserActor = NULL;
	this->_pEnemyUserActor = NULL;
	this->_pBoardInterface = NULL;
	this->_pBoardStatus = NULL;

	this->_pUseEvaluator = NULL;
	this->_pLearnBoardEvaluator = NULL;
	this->_pStoneDiffEvaluator = NULL;
}