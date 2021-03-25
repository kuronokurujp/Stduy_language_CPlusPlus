#include "Scene/Game/Evaluator/learn_borad_placement_stone_evaluator.h"

#include "common.h"
#include "Scene/Game/Actor/user_actor.h"

unsigned int LearnBoardPlacementStoneEvaluator::_sEdgeTableSize = 0;
LearnBoardPlacementStoneEvaluator::EdgeStat* LearnBoardPlacementStoneEvaluator::_spEdgeTable = NULL;
int LearnBoardPlacementStoneEvaluator::_sTableInitCount = 0;

// 標準ではint型をサポートしていないので用意
static int StaticPowSimple(int x, int y)
{
	unsigned int a = 1;
	for (int i = 0; i < y; ++i)
	{
		a *= x;
	}

	return a;
}

LearnBoardPlacementStoneEvaluator::LearnBoardPlacementStoneEvaluator()
{
	this->_Clear();
}

LearnBoardPlacementStoneEvaluator::~LearnBoardPlacementStoneEvaluator()
{
	SAFETY_MEM_ARRAY_RELEASE(this->_pTempWorkBufferLine);

	--LearnBoardPlacementStoneEvaluator::_sTableInitCount;
	if (LearnBoardPlacementStoneEvaluator::_sTableInitCount <= 0)
	{
		SAFETY_MEM_ARRAY_RELEASE(LearnBoardPlacementStoneEvaluator::_spEdgeTable);
	}
}

const int LearnBoardPlacementStoneEvaluator::Evaluator(
	BoardStatusInteface* in_pBoardStatus, UserActor* in_pCurrentUserActor)
{
	this->_pBoardStatus = in_pBoardStatus;

	// テーブルデータは初回のみ作成で良い
	if (this->_initTableFlag == false)
	{
		this->_initTableFlag = true;

		unsigned int size = in_pBoardStatus->GetBoardSide();

		this->_pTempWorkBufferLine = new BoardData::eStone[size];
		memset(this->_pTempWorkBufferLine, BoardData::eStone_None, sizeof(BoardData::eStone) * size);

		if (LearnBoardPlacementStoneEvaluator::_sTableInitCount <= 0)
		{
			// 1辺の石を置いた全パターンを作成
			LearnBoardPlacementStoneEvaluator::_sEdgeTableSize = StaticPowSimple(BoardData::eStone_ColorTypeMax, size);
			LearnBoardPlacementStoneEvaluator::_spEdgeTable = new EdgeStat[LearnBoardPlacementStoneEvaluator::_sEdgeTableSize];

			BoardData::eStone* pTempWorkBufferLine = new BoardData::eStone[size];
			memset(pTempWorkBufferLine, BoardData::eStone_None, sizeof(BoardData::eStone) * size);

			this->_CreateEdgeTableData(pTempWorkBufferLine, 0);

			SAFETY_MEM_ARRAY_RELEASE(pTempWorkBufferLine);
		}

		++LearnBoardPlacementStoneEvaluator::_sTableInitCount;
	}

	// 重み値を設定
	{
		_evaluationWeight._mobility = 67;
		_evaluationWeight._liberty = -13;
		_evaluationWeight._stable = 101;
		_evaluationWeight._wing = -308;
		_evaluationWeight._xmove = -449;
		_evaluationWeight._cmove = -552;
	}

	EdgeStat edgestat;
	CornerStat cornerstat;
	int result;

	unsigned int idxTopIndex = _GetIndexTopSide();
	unsigned int idxBottomIndex = _GetIndexBottomSide();
	unsigned int idxRightIndex = _GetIndexRightSide();
	unsigned int idxLeftIndex = _GetIndexLeftSide();

	edgestat = LearnBoardPlacementStoneEvaluator::_spEdgeTable[idxTopIndex];
	edgestat += LearnBoardPlacementStoneEvaluator::_spEdgeTable[idxBottomIndex];
	edgestat += LearnBoardPlacementStoneEvaluator::_spEdgeTable[idxRightIndex];
	edgestat += LearnBoardPlacementStoneEvaluator::_spEdgeTable[idxLeftIndex];

	cornerstat = _GetEvaluationCorner();

	edgestat[BoardData::eStone_ColorBlack]._statble -= cornerstat[BoardData::eStone_ColorBlack]._corner;
	edgestat[BoardData::eStone_ColorWhite]._statble -= cornerstat[BoardData::eStone_ColorWhite]._corner;

	result =
		edgestat[BoardData::eStone_ColorBlack]._statble * _evaluationWeight._stable
		- edgestat[BoardData::eStone_ColorWhite]._statble * _evaluationWeight._stable

		+ edgestat[BoardData::eStone_ColorBlack]._wing * _evaluationWeight._wing
		- edgestat[BoardData::eStone_ColorWhite]._wing * _evaluationWeight._wing

		+ edgestat[BoardData::eStone_ColorBlack]._wing * _evaluationWeight._xmove
		- edgestat[BoardData::eStone_ColorWhite]._wing * _evaluationWeight._xmove

		+ edgestat[BoardData::eStone_ColorBlack]._wing * _evaluationWeight._cmove
		- edgestat[BoardData::eStone_ColorWhite]._wing * _evaluationWeight._cmove
		;

	if (_evaluationWeight._liberty != 0)
	{
		StoneTypeDataStorage<unsigned int> liberty = _GetCountLiberty();
		result += liberty[BoardData::eStone_ColorBlack] * _evaluationWeight._liberty;
		result += liberty[BoardData::eStone_ColorWhite] * _evaluationWeight._liberty;
	}

	// 手番の石を基準に評価値に重みを付ける
	std::vector<BoardData::sPoint> placementPosList = in_pBoardStatus->GetPlacementStonePointList(in_pCurrentUserActor->GetUseStone());

	result +=
		in_pBoardStatus->GetPlaceStoneCount(in_pCurrentUserActor->GetUseStone())
		* static_cast<int>(placementPosList.size())
		* _evaluationWeight._mobility;

	return in_pCurrentUserActor->GetUseStone() * result;
}

void LearnBoardPlacementStoneEvaluator::_CreateEdgeTableData(BoardData::eStone in_aTempWorkBufferLine[], unsigned int in_index)
{
	unsigned int size = this->_pBoardStatus->GetBoardSide();
	if (in_index >= size)
	{
		EdgeStat stat;
		stat[BoardData::eStone::eStone_ColorBlack] = _GetEvaluationEdge(in_aTempWorkBufferLine, BoardData::eStone::eStone_ColorBlack);
		stat[BoardData::eStone::eStone_ColorWhite] = _GetEvaluationEdge(in_aTempWorkBufferLine, BoardData::eStone::eStone_ColorWhite);

		unsigned int tableIndex = _GetIndexLine(in_aTempWorkBufferLine);
		assert(tableIndex < LearnBoardPlacementStoneEvaluator::_sEdgeTableSize);

		LearnBoardPlacementStoneEvaluator::_spEdgeTable[tableIndex] = stat;

		return;
	}

	in_aTempWorkBufferLine[in_index] = BoardData::eStone::eStone_None;
	_CreateEdgeTableData(in_aTempWorkBufferLine, in_index + 1);

	in_aTempWorkBufferLine[in_index] = BoardData::eStone::eStone_ColorWhite;
	_CreateEdgeTableData(in_aTempWorkBufferLine, in_index + 1);

	in_aTempWorkBufferLine[in_index] = BoardData::eStone::eStone_ColorBlack;
	_CreateEdgeTableData(in_aTempWorkBufferLine, in_index + 1);
}

unsigned int LearnBoardPlacementStoneEvaluator::_GetIndexTopSide() const
{
	int max = static_cast<int>(this->_pBoardStatus->GetBoardSide());
	for (int i = 0; i < max; ++i)
	{
		this->_pTempWorkBufferLine[i] = this->_pBoardStatus->GetPlaceStoneType(i, 0);
	}

	return this->_GetIndexLine(this->_pTempWorkBufferLine);
}

unsigned int LearnBoardPlacementStoneEvaluator::_GetIndexBottomSide() const
{
	int max = static_cast<int>(this->_pBoardStatus->GetBoardSide());
	for (int i = 0; i < max; ++i)
	{
		this->_pTempWorkBufferLine[i] =
			this->_pBoardStatus->GetPlaceStoneType(i, max - 1);
	}

	return this->_GetIndexLine(this->_pTempWorkBufferLine);
}

unsigned int LearnBoardPlacementStoneEvaluator::_GetIndexRightSide() const
{
	int max = static_cast<int>(this->_pBoardStatus->GetBoardSide());
	for (int i = 0; i < max; ++i)
	{
		this->_pTempWorkBufferLine[i] =
			this->_pBoardStatus->GetPlaceStoneType(max - 1, i);
	}

	return this->_GetIndexLine(this->_pTempWorkBufferLine);
}

unsigned int LearnBoardPlacementStoneEvaluator::_GetIndexLeftSide() const
{
	int max = static_cast<int>(this->_pBoardStatus->GetBoardSide());
	for (int i = 0; i < max; ++i)
	{
		this->_pTempWorkBufferLine[i] = this->_pBoardStatus->GetPlaceStoneType(0, i);
	}

	return this->_GetIndexLine(this->_pTempWorkBufferLine);
}

unsigned int LearnBoardPlacementStoneEvaluator::_GetIndexLine(BoardData::eStone in_line[]) const
{
	unsigned int sum = 0;
	int max = static_cast<int>(this->_pBoardStatus->GetBoardSide());
	for (int i = 1; i < max; ++i)
	{
		sum += (StaticPowSimple(BoardData::eStone_ColorTypeMax, i) * (in_line[i] + 1));
	}
	sum += in_line[0] + 1;

	return sum;
}

LearnBoardPlacementStoneEvaluator::CornerStat LearnBoardPlacementStoneEvaluator::_GetEvaluationCorner()
{
	CornerStat cornerstat;

	cornerstat[BoardData::eStone_ColorBlack]._corner = 0;
	cornerstat[BoardData::eStone_ColorBlack]._xmove = 0;
	cornerstat[BoardData::eStone_ColorWhite]._corner = 0;
	cornerstat[BoardData::eStone_ColorWhite]._xmove = 0;

	auto boardWidth = this->_pBoardStatus->GetBoardSide();
	auto boardHeight = this->_pBoardStatus->GetBoardSide();

	// 左上隅の石の情報
	auto stone = this->_pBoardStatus->GetPlaceStoneType(0, 0);
	cornerstat[stone]._corner++;
	if (stone == BoardData::eStone_None)
	{
		++cornerstat[this->_pBoardStatus->GetPlaceStoneType(1, 1)]._xmove;
	}

	// 左下隅の石の情報
	stone = this->_pBoardStatus->GetPlaceStoneType(0, boardHeight - 1);
	cornerstat[stone]._corner++;
	if (stone == BoardData::eStone_None)
	{
		++cornerstat[this->_pBoardStatus->GetPlaceStoneType(1, boardHeight - 2)]._xmove;
	}

	// 右上隅の石の情報
	stone = this->_pBoardStatus->GetPlaceStoneType(boardWidth - 1, boardHeight - 1);
	cornerstat[stone]._corner++;
	if (stone == BoardData::eStone_None)
	{
		++cornerstat[this->_pBoardStatus->GetPlaceStoneType(boardWidth - 2, boardHeight - 2)]._xmove;
	}

	// 右下隅の石の情報
	stone = this->_pBoardStatus->GetPlaceStoneType(boardWidth - 1, 0);
	cornerstat[stone]._corner++;
	if (stone == BoardData::eStone_None)
	{
		++cornerstat[this->_pBoardStatus->GetPlaceStoneType(boardWidth - 2, 1)]._xmove;
	}

	return cornerstat;
}

LearnBoardPlacementStoneEvaluator::EdgeParam LearnBoardPlacementStoneEvaluator::_GetEvaluationEdge(BoardData::eStone in_aLine[], BoardData::eStone in_stone) const
{
	EdgeParam edgeparam;

	auto boardWidth = this->_pBoardStatus->GetBoardSide();

	// 辺の両端に石が置いてない場合、ウィングによる評価計算をする
	if (in_aLine[0] == BoardData::eStone_None && in_aLine[boardWidth - 1] == BoardData::eStone_None)
	{
		unsigned int lineIndex = 0;
		// 左から右のマス目に配置している石が引数と異なる石かチェック
		lineIndex = 2;
		while (lineIndex <= (boardWidth - 3))
		{
			if (in_aLine[lineIndex] != in_stone) break;
			++lineIndex;
		}

		// ブロック状態になっている
		if (lineIndex == (boardWidth - 2))
		{
			// 石の並びに応じてウィンドウ・山の判定
			if (in_aLine[1] == in_stone && in_aLine[boardWidth - 2] == BoardData::eStone_None)
			{
				edgeparam._wing = 1;
			}
			else if (in_aLine[1] == BoardData::eStone_None && in_aLine[boardWidth - 2] == in_stone)
			{
				edgeparam._wing = 1;
			}
			else if (in_aLine[1] == in_stone && in_aLine[boardWidth - 2] == in_stone)
			{
				edgeparam._mountain = 1;
			}
		}
		else
		{
			// C打ち判定
			if (in_aLine[1] == in_stone)
			{
				edgeparam._cmove++;
			}

			if (in_aLine[boardWidth - 2] == in_stone)
			{
				edgeparam._cmove++;
			}
		}
	}

	// 隅から置いている石が連続している場合は確定石とする
	{
		// 左隅から右隅へ探索
		for (unsigned int i = 0; i < boardWidth; ++i)
		{
			if (in_aLine[i] != in_stone) break;
			++edgeparam._statble;
		}

		// 右隅から左隅へ探索
		if (static_cast<unsigned int>(edgeparam._statble) < boardWidth)
		{
			for (int i = boardWidth - 1; i > 0; --i)
			{
				if (in_aLine[i] != in_stone) break;
				++edgeparam._statble;
			}
		}
	}

	return edgeparam;
}

StoneTypeDataStorage<unsigned int> LearnBoardPlacementStoneEvaluator::_GetCountLiberty() const
{
	StoneTypeDataStorage<unsigned int> liberty;

	liberty[BoardData::eStone_ColorBlack] = 0;
	liberty[BoardData::eStone_ColorWhite] = 0;
	liberty[BoardData::eStone_None] = 0;

	int stone = 0;
	for (unsigned int x = 0; x < this->_pBoardStatus->GetBoardSide(); ++x)
	{
		for (unsigned int y = 0; y < this->_pBoardStatus->GetBoardSide(); ++y)
		{
			stone = static_cast<int>(this->_pBoardStatus->GetPlaceStoneType(x, y));
			liberty[stone] += this->_pBoardStatus->GetLibertry(x, y);
		}
	}

	return liberty;
}

void LearnBoardPlacementStoneEvaluator::_Clear()
{
	this->_pBoardStatus = NULL;
	this->_pTempWorkBufferLine = NULL;
	this->_initTableFlag = false;
}