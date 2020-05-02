#ifndef __LEARN_BOARD_PLACEMENT_STONE_EVALUATOR_H__
#define __LEARN_BOARD_PLACEMENT_STONE_EVALUATOR_H__

#include "Interface/placementStoneEvaluatorInterface.h"
#include "Interface/boardInterface.h"

/// <summary>
/// 盤を学習して評価.
/// </summary>
class LearnBoardPlacementStoneEvaluator : public PlacementStoneEvaluatorInterface
{
public:
	LearnBoardPlacementStoneEvaluator();
	~LearnBoardPlacementStoneEvaluator();

	const int Evaluator(
		BoardStatusInteface* in_pBoardStatus, UserActor* in_pCurrentUserActor);

private:
	struct EdgeParam
	{
		EdgeParam()
		{
			this->_statble = 0;
			this->_wing = 0;
			this->_mountain = 0;
			this->_cmove = 0;
		}

		EdgeParam& operator +=(const EdgeParam in_src)
		{
			this->_statble += in_src._statble;
			this->_wing += in_src._wing;
			this->_mountain += in_src._mountain;
			this->_cmove += in_src._cmove;

			return *this;
		}

		short _statble;
		short _wing;
		short _mountain;
		short _cmove;
	};

	struct CornerParma
	{
		CornerParma()
		{
			this->_corner = this->_xmove = 0;
		}

		short _corner;
		short _xmove;
	};

	// パラメーターの重み
	struct Weight
	{
		Weight()
		{
			this->_mobility = 0;
			this->_liberty = 0;
			this->_stable = 0;
			this->_wing = 0;
			this->_xmove = 0;
			this->_cmove = 0;
		}

		int _mobility;
		int _liberty;
		int _stable;
		int _wing;
		int _xmove;
		int _cmove;
	};

	typedef StoneTypeDataStorage<EdgeParam> EdgeStat;
	typedef StoneTypeDataStorage<CornerParma> CornerStat;

	/// <summary>
	/// 辺の全ての石の並びのパターンに対してのデータ作成.
	/// </summary>
	void _CreateEdgeTableData(BoardData::eStone in_aTempWorkBufferLine[], unsigned int in_index);

	/// <summary>
	/// 指定した石と石を置いた辺から端の評価データを取得.
	/// </summary>
	/// <returns></returns>
	EdgeParam _GetEvaluationEdge(BoardData::eStone in_aLine[], BoardData::eStone in_stone) const;

	/// <summary>
	/// 盤の隅の評価データを取得.
	/// </summary>
	/// <returns></returns>
	CornerStat _GetEvaluationCorner();

	/// <summary>
	/// 盤のマス目の解放度データを取得.
	/// </summary>
	/// <returns></returns>
	StoneTypeDataStorage<unsigned> _GetCountLiberty() const;

	inline unsigned int _GetIndexTopSide() const;
	inline unsigned int _GetIndexBottomSide() const;
	inline unsigned int _GetIndexRightSide() const;
	inline unsigned int _GetIndexLeftSide() const;
	unsigned int _GetIndexLine(BoardData::eStone in_line[]) const;

	void _Clear();

	static unsigned int _sEdgeTableSize;
	static EdgeStat* _spEdgeTable;
	static int _sTableInitCount;

	Weight _evaluationWeight;
	bool _initTableFlag;

	BoardStatusInteface* _pBoardStatus;
	BoardData::eStone* _pTempWorkBufferLine;
};

#endif // __LEARN_BOARD_PLACEMENT_STONE_EVALUATOR_H__
