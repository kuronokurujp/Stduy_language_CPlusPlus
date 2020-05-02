#include "Actor/boardActor.h"

#include <assert.h>

#include "Component/boardRenderComponent.h"
#include "common.h"

// 石を置く各方向
static const BoardData::sPoint s_aPlacementStoneDir[] =
{
	// 上、右上、左下
	BoardData::sPoint(0, -1),
	BoardData::sPoint(1, -1),
	BoardData::sPoint(-1, -1),

	// 左右
	BoardData::sPoint(1, 0),
	BoardData::sPoint(-1, 0),

	// 下、右下、左下
	BoardData::sPoint(0, 1),
	BoardData::sPoint(1, 1),
	BoardData::sPoint(-1, 1),
};

BoardActor::BoardActor(ConsoleRenderingInterface* in_pRendering)
{
	assert(in_pRendering != NULL);

	this->_Clear();

	// 盤の描画コンポーネント設定
	this->_pRenderComponent = new BoardRenderComponent(this, in_pRendering);

	this->_whiteStonePlacementPositions.reserve(eBoardSquaresCount_Max);
	this->_blackStonePlacementPositions.reserve(eBoardSquaresCount_Max);
	this->_tempWorkflipStonePositions.reserve(eBoardSquaresCount_Max);

	this->Reset();
}

void BoardActor::UpdateActor(float in_deltaTimeSecond)
{
	// 石数をレンダーに設定
	this->_pRenderComponent->SetStoneCount(BoardData::eStone_ColorBlack, this->_blackStoneCount);
	this->_pRenderComponent->SetStoneCount(BoardData::eStone_ColorWhite, this->_whiteStoneCount);
}

/// <summary>
/// 盤の状態をリセット.
/// </summary>
void BoardActor::Reset()
{
	memset(this->_aaSquares, BoardData::eStone_None, sizeof(this->_aaSquares));

	this->_tempWorkflipStonePositions.clear();
	this->_placementHistory.clear();

	this->_aaSquares[3][3] = BoardData::eStone_ColorWhite;
	this->_aaSquares[4][3] = BoardData::eStone_ColorBlack;
	this->_aaSquares[3][4] = BoardData::eStone_ColorBlack;
	this->_aaSquares[4][4] = BoardData::eStone_ColorWhite;

	this->_UpdateStoneInfo();
}

/// <summary>
/// 石を置くコマンド(座標指定).
/// </summary>
BoardControllerInteface::eResultCommand BoardActor::CommandPlacementStone(const BoardData::sPoint& in_rBoardPoint, const BoardData::eStone in_stone)
{
	BoardData::sPoint point(in_rBoardPoint._x, in_rBoardPoint._y);
	assert(point._x < eBoardSquaresCount_Side);
	assert(point._y < eBoardSquaresCount_Side);
	assert(point._x >= 0);
	assert(point._y >= 0);

	// 石がすでに置いてあるか
	if (this->_aaSquares[point._y][point._x] != BoardData::eStone::eStone_None)
	{
		return eResultCommand_PlacementStoneMiss;
	}

	// 置いた石がひっくり返せるか
	std::vector<BoardData::sPoint> stonePositions;
	stonePositions.reserve(eBoardSquaresCount_Side * eBoardSquaresCount_Side);
	{
		this->_tempWorkflipStonePositions.clear();
		for (unsigned int i = 0; i < StaticSingleArrayLength(s_aPlacementStoneDir); ++i)
		{
			this->_OutputFlipStonePositions(this->_tempWorkflipStonePositions, point, s_aPlacementStoneDir[i], in_stone);
			if (this->_tempWorkflipStonePositions.size() > 0)
			{
				std::copy(
					this->_tempWorkflipStonePositions.begin(),
					this->_tempWorkflipStonePositions.end(),
					std::back_inserter(stonePositions));
			}
		}
	}

	if (stonePositions.size() <= 0)
	{
		// 石をひっくり返せるのがないなら終了
		return eResultCommand_FlipStoneMiss;
	}

	// コマンドとして記録
	sCommandDataPlaceStone commandData;
	commandData._placeStone = in_stone;
	memcpy(commandData._aaOldBoardSquares, this->_aaSquares, sizeof(this->_aaSquares));
	memcpy(commandData._aaOldSquaresLibetry, this->_aaSquaresLibetry, sizeof(this->_aaSquaresLibetry));
	this->_placementHistory.push_back(commandData);

	// 石を置く
	this->_aaSquares[point._y][point._x] = in_stone;

	// 石を置いたマスを中心に四方のマスの解放度を更新　
	{
		int boardWidth = static_cast<int>(this->GetBoardSide());
		int boardHeight = static_cast<int>(this->GetBoardSide());

		for (int y = point._y - 1; y <= point._y + 1; ++y)
		{
			// 盤外かどうかチェック
			if (y < 0) continue;
			if (y >= boardHeight) break;

			for (int x = point._x - 1; x <= point._x + 1; ++x)
			{
				// 盤外かどうかチェック
				if (x < 0) continue;
				if (x >= boardWidth) break;

				--this->_aaSquaresLibetry[y][x];
			}
		}
	}

	// 石をひっくり返す
	for (auto flipStonePosition : stonePositions)
	{
		this->_aaSquares[flipStonePosition._y][flipStonePosition._x] = in_stone;
	}

	this->_UpdateStoneInfo();

	return eResultCommand_Success;
}

/// <summary>
/// 石を置くコマンド(アルファベットとライン番号指定).
/// </summary>
BoardControllerInteface::eResultCommand BoardActor::CommandPlacementStone(const char in_alphabet, const unsigned int in_lineNumber, const BoardData::eStone in_stone)
{
	assert(0 < in_lineNumber);
	assert(in_lineNumber <= eBoardSquaresCount_Side);

	// アルファベットを列数に変える
	// ライン数を行数に変える
	unsigned int x, y;
	static const std::string alfpabet("abcdefgh");
	x = static_cast<unsigned int>(alfpabet.find_first_of(in_alphabet));
	y = in_lineNumber - 1;

	BoardData::sPoint point(x, y);
	return this->CommandPlacementStone(point, in_stone);
}

// 石を置いたコマンドのundo
bool BoardActor::CommandUndoPlacement(const BoardData::eStone in_stone)
{
	if (this->_placementHistory.empty())
	{
		return false;
	}

	// 履歴の末尾から指定した石までの位置に戻す
	auto iter = this->_placementHistory.rbegin();

	bool undoHitIterFlag = false;
	if (iter->_placeStone == in_stone)
	{
		undoHitIterFlag = true;
	}
	else
	{
		while (iter != this->_placementHistory.rend())
		{
			if (iter->_placeStone != in_stone)
			{
				this->_placementHistory.pop_back();
				iter = this->_placementHistory.rbegin();
			}
			else
			{
				undoHitIterFlag = true;
				break;
			}
		}
	}

	// 履歴までの配置石にする
	if (undoHitIterFlag == true)
	{
		memcpy(this->_aaSquares, iter->_aaOldBoardSquares, sizeof(this->_aaSquares));
		memcpy(this->_aaSquaresLibetry, iter->_aaOldSquaresLibetry, sizeof(this->_aaSquaresLibetry));

		this->_UpdateStoneInfo();

		this->_placementHistory.pop_back();
	}

	return undoHitIterFlag;
}

/// <summary>
/// 石の個数.
/// </summary>
const int BoardActor::GetPlaceStoneCount(const BoardData::eStone in_stone) const
{
	switch (in_stone)
	{
	case BoardData::eStone_ColorBlack: return this->_blackStoneCount;
	case BoardData::eStone_ColorWhite: return this->_whiteStoneCount;
	}

	assert(0);
	return 0;
}

/// <summary>
/// 指定した石の文字コード.
/// </summary>
const char BoardActor::GetStoneCharacterCode(const BoardData::eStone in_stone) const
{
	return this->_pRenderComponent->GetStoneCharacterCode(in_stone);
}

/// <summary>
/// 指定位置の石種類を取得.
/// </summary>
inline const BoardData::eStone BoardActor::GetPlaceStoneType(int in_x, int in_y)
{
	assert(in_x < eBoardSquaresCount_Side);
	assert(in_y < eBoardSquaresCount_Side);
	assert(in_x >= 0);
	assert(in_y >= 0);

	return this->_aaSquares[in_y][in_x];
}

// 指定したマスの開放度取得
int BoardActor::GetLibertry(int in_x, int in_y)
{
	assert(in_x < eBoardSquaresCount_Side);
	assert(in_y < eBoardSquaresCount_Side);
	assert(in_x >= 0);
	assert(in_y >= 0);

	return this->_aaSquaresLibetry[in_y][in_x];
}

/// <summary>
/// 石がもう置けない状態かどうか.
/// </summary>
bool BoardActor::IsCannotPlacementStone()
{
	// 黒白両方が石を置けない
	bool whiteStonePassFlag = false;
	bool blackStonePassFlag = false;
	{
		blackStonePassFlag = !(this->IsPlacementStone(BoardData::eStone_ColorBlack));
		whiteStonePassFlag = !(this->IsPlacementStone(BoardData::eStone_ColorWhite));
	}

	// 全ての石が埋まっているか
	bool fullPlacementStoneFlag = true;
	{
		for (unsigned int y = 0; y < eBoardSquaresCount_Side; ++y)
		{
			for (unsigned int x = 0; x < eBoardSquaresCount_Side; ++x)
			{
				if (this->_aaSquares[y][x] == BoardData::eStone_None)
				{
					fullPlacementStoneFlag = false;
					break;
				}
			}

			if (fullPlacementStoneFlag == false)
			{
				break;
			}
		}
	}

	// 全て石が埋まっているのでこれ以上石が置けない
	if (fullPlacementStoneFlag == true)
	{
		return true;
	}

	// 白黒と両方が石が置けないならもう終わる
	if (whiteStonePassFlag && blackStonePassFlag)
	{
		return true;
	}

	return false;
}

// 指定した石が置けるかチェック
bool BoardActor::IsPlacementStone(const BoardData::eStone in_stone)
{
	switch (in_stone)
	{
	case BoardData::eStone_ColorBlack: return this->_blackStonePlacementPositions.size() > 0;
	case BoardData::eStone_ColorWhite: return this->_whiteStonePlacementPositions.size() > 0;
	}

	assert(0);
	return false;
}

// 指定した石が盤に置ける座標リストを取得
const std::vector<BoardData::sPoint>& BoardActor::GetPlacementStonePointList(const BoardData::eStone in_stone)
{
	switch (in_stone)
	{
	case BoardData::eStone_ColorBlack: return this->_blackStonePlacementPositions;
	case BoardData::eStone_ColorWhite: return this->_whiteStonePlacementPositions;
	}

	assert(0);
	return this->_blackStonePlacementPositions;
}

// 指定した石が盤に置ける座標リストを出力
void BoardActor::_OutputPlacementStonePosList(
	std::vector<BoardData::sPoint>& out_rPosList, const BoardData::eStone in_stone)
{
	this->_tempWorkflipStonePositions.clear();
	for (unsigned int y = 0; y < eBoardSquaresCount_Side; ++y)
	{
		for (unsigned int x = 0; x < eBoardSquaresCount_Side; ++x)
		{
			BoardData::sPoint point(x, y);
			// 空いているかどうかチェック
			if (this->_aaSquares[point._y][point._x] != BoardData::eStone_None)
			{
				continue;
			}

			// 石がひっくり返せるかチェック
			for (unsigned int dirIndex = 0; dirIndex < StaticSingleArrayLength(s_aPlacementStoneDir); ++dirIndex)
			{
				this->_OutputFlipStonePositions(this->_tempWorkflipStonePositions, point, s_aPlacementStoneDir[dirIndex], in_stone);
				// ひっくり返せる石があれば、石が置ける！
				if (this->_tempWorkflipStonePositions.size() > 0)
				{
					out_rPosList.push_back(point);
					break;
				}
			}
		}
	}
}

/// <summary>
/// ひっくり返せる石の位置を出力.
/// </summary>
void BoardActor::_OutputFlipStonePositions(
	std::vector<BoardData::sPoint>& out_rFlipStonePositions, const BoardData::sPoint& in_rPoint, const BoardData::sPoint& in_rSearchDir, const BoardData::eStone in_stone)
{
	out_rFlipStonePositions.clear();

	BoardData::sPoint searchPoint(in_rPoint);

	BoardData::eStone checkStone = BoardData::eStone_None;
	switch (in_stone)
	{
	case BoardData::eStone_ColorBlack: checkStone = BoardData::eStone_ColorWhite; break;
	case BoardData::eStone_ColorWhite: checkStone = BoardData::eStone_ColorBlack; break;
	}

	while (true)
	{
		searchPoint._x += in_rSearchDir._x;
		searchPoint._y += in_rSearchDir._y;

		// 範囲外かチェック
		if ((searchPoint._x < 0) || (searchPoint._y < 0))
		{
			// 範囲外は挟める石がないのでひっくり返す石のリストはクリア
			out_rFlipStonePositions.clear();
			break;
		}
		else if ((searchPoint._x >= eBoardSquaresCount_Side) || (searchPoint._y >= eBoardSquaresCount_Side))
		{
			// 範囲外は挟める石がないのでひっくり返す石のリストはクリア
			out_rFlipStonePositions.clear();
			break;
		}

		// 挟む石があるかチェック
		auto stone = this->_aaSquares[searchPoint._y][searchPoint._x];
		if (checkStone == stone)
		{
			// ひっくり返す石がある
			out_rFlipStonePositions.push_back(searchPoint);
		}
		else if (in_stone == stone)
		{
			// 挟む石があるので終端
			break;
		}
		else if (stone == BoardData::eStone_None)
		{
			// 石が存在しないので終端
			// ひっくり返す石は存在しない
			out_rFlipStonePositions.clear();
			break;
		}
	}
}

/// <summary>
/// 置いている石の数.
/// </summary>
const int BoardActor::_GetPlacementStoneCount(BoardData::eStone in_stone)
{
	int stoneCount = 0;
	// 二次元配列を一次元配列として扱う
	BoardData::eStone* pSquares = this->_aaSquares[0];
	for (unsigned int i = 0; i < eBoardSquaresCount_Max; ++i)
	{
		if (pSquares[i] == in_stone)
		{
			++stoneCount;
		}
	}

	return stoneCount;
}

/// <summary>
/// 石に関連する情報更新.
/// </summary>
void BoardActor::_UpdateStoneInfo()
{
	// 石の数を計算
	this->_blackStoneCount = this->_GetPlacementStoneCount(BoardData::eStone_ColorBlack);
	this->_whiteStoneCount = this->_GetPlacementStoneCount(BoardData::eStone_ColorWhite);

	// 置ける石のリスト更新
	this->_blackStonePlacementPositions.clear();
	this->_OutputPlacementStonePosList(this->_blackStonePlacementPositions, BoardData::eStone_ColorBlack);

	this->_whiteStonePlacementPositions.clear();
	this->_OutputPlacementStonePosList(this->_whiteStonePlacementPositions, BoardData::eStone_ColorWhite);
}

void BoardActor::_Clear()
{
	memset(this->_aaSquares, BoardData::eStone_None, sizeof(this->_aaSquares));
	memset(this->_aaSquaresLibetry, BoardData::eStone_None, sizeof(this->_aaSquaresLibetry));

	this->_placementHistory.clear();
	this->_tempWorkflipStonePositions.clear();
	this->_pRenderComponent = NULL;
	this->_blackStoneCount = this->_whiteStoneCount = 0;
}