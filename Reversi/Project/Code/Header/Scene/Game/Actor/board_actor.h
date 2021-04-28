#ifndef __BOARD_ACTOR_H__
#define __BOARD_ACTOR_H__

#include "Actor/actor.h"
#include "Scene/Game/Interface/board_Interface.h"

#include <vector>

// 前方宣言
class RenderingInterface;
class BoardRenderComponent;

/// <summary>
/// リバーシーの盤
/// </summary>
class BoardActor final : public Actor, public BoardControllerInteface, public BoardStatusInteface
{
public:
	// 盤の縦横のマス目の数.
	enum eBoardSquareCount
	{
		eBoardSquaresCount_Side = 8,

		eBoardSquaresCount_Max = eBoardSquaresCount_Side * eBoardSquaresCount_Side,
	};

	// 石を打ったコマンドデータ.
	struct sCommandDataPlaceStone
	{
		BoardData::eStone _placeStone;
		BoardData::eStone _aaOldBoardSquares[eBoardSquaresCount_Side][eBoardSquaresCount_Side];
		BoardData::eStone _aaOldSquaresLibetry[eBoardSquaresCount_Side][eBoardSquaresCount_Side];
	};

	BoardActor(RenderingInterface* in_pRendering);

	void UpdateActor(float in_deltaTimeSecond) override final;

	/// <summary>
	/// 盤の状態をリセット.
	/// </summary>
	void Reset();

	/// <summary>
	/// 石を置くコマンド(座標指定).
	/// 引数の座標値は0が基準でなく1なので注意.
	/// </summary>
	eResultCommand CommandPlacementStone(const BoardData::sPoint& in_rBoardPoint, const BoardData::eStone in_stone) override final;

	/// <summary>
	/// 石を置くコマンド(アルファベットとライン番号指定).
	/// </summary>
	eResultCommand CommandPlacementStone(const char in_alphabet, const unsigned int in_lineNumber, const BoardData::eStone in_stone) override final;

	// 石を置いたコマンドのundo
	bool CommandUndoPlacement(const BoardData::eStone in_stone);

	// 盤の1辺のマス目の数
	inline const unsigned int GetBoardSide() const override final { return eBoardSquaresCount_Side; }

	/// <summary>
	/// 石の個数.
	/// </summary>
	const int GetPlaceStoneCount(const BoardData::eStone in_stone) const override final;

	/// <summary>
	/// 指定した石の文字コード.
	/// </summary>
	const char GetStoneCharacterCode(BoardData::eStone in_stone) const override final;

	/// <summary>
	/// 指定位置の石種類を取得.
	/// </summary>
	inline const BoardData::eStone GetPlaceStoneType(int in_x, int in_y) override final;

	// 指定したマスの開放度取得
	int GetLibertry(int in_x, int in_y) override final;

	/// <summary>
	/// 石がもう置けない状態.
	/// </summary>
	bool IsCannotPlacementStone() override final;

	/// <summary>
	/// 指定した石が置けるかチェック.
	/// </summary>
	bool IsPlacementStone(const BoardData::eStone in_stone) override final;

	// 指定した石が盤に置ける座標リストを出力
	const std::vector<BoardData::sPoint>& GetPlacementStonePointList(const BoardData::eStone in_stone) override final;

private:
	// 指定した石が盤に置ける座標リストを出力
	void _OutputPlacementStonePosList(
		std::vector<BoardData::sPoint>& out_rPosList, const BoardData::eStone in_stone);

	/// <summary>
	/// ひっくり返せる石の位置を出力.
	/// </summary>
	void _OutputFlipStonePositions(
		std::vector<BoardData::sPoint>& out_rFlipStonePositions,
		const BoardData::sPoint& in_rPoint,
		const BoardData::sPoint& in_rSearchDir,
		const BoardData::eStone in_stone);

	/// <summary>
	/// 置いている石の数.
	/// </summary>
	const int _GetPlacementStoneCount(BoardData::eStone in_stone);

	/// <summary>
	/// 石に関連する情報更新.
	/// </summary>
	void _UpdateStoneInfo();

	void _Clear();

private:

	BoardRenderComponent* _p_render_component;

	BoardData::eStone _aa_squares[eBoardSquaresCount_Side][eBoardSquaresCount_Side];
	// 各マスの開放度値
	int _aa_squares_libetry[eBoardSquaresCount_Side][eBoardSquaresCount_Side];

	// 打った手の記録.
	std::vector<sCommandDataPlaceStone> _placement_history;

	// 石をひっくり返す処理をする時の作業バッファ
	std::vector<BoardData::sPoint> _tempwork_flip_stone_positions;

	unsigned int _black_stone_count, _white_stone_count;
	std::vector<BoardData::sPoint> _black_stone_placement_positions, _white_stone_placement_positions;
};

#endif // __BOARD_ACTOR_H__
