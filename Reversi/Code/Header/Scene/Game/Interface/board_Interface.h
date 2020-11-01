#ifndef __BOARD_INTERFACE_H__
#define __BOARD_INTERFACE_H__

#include <vector>
#include <assert.h>

namespace BoardData
{
	// 石の種類
	enum eStone
	{
		eStone_ColorWhite = -1,
		eStone_None = 0,
		eStone_ColorBlack = 1,

		eStone_ColorTypeMax = 3,
	};

	struct sPoint
	{
		sPoint()
		{
			this->_x = this->_y = 0;
		}

		sPoint(const int in_x, const int in_y)
		{
			this->_x = in_x;
			this->_y = in_y;
		}

		int _x, _y;
	};
}

/// <summary>
/// 盤の制御インターフェイス
/// </summary>
class BoardControllerInteface
{
public:
	enum eResultCommand
	{
		eResultCommand_Success,
		eResultCommand_PlacementStoneMiss,
		eResultCommand_FlipStoneMiss,
	};

	// 石を置くコマンド
	virtual eResultCommand CommandPlacementStone(const BoardData::sPoint& in_rPoint, const BoardData::eStone in_stone) = 0;
	virtual eResultCommand CommandPlacementStone(const char in_alphabet, const unsigned int in_lineNumber, const BoardData::eStone in_stone) = 0;

	// 石を置いたコマンドのundo
	virtual bool CommandUndoPlacement(const BoardData::eStone in_stone) = 0;
};

/// <summary>
/// 盤の状態のインターフェイス
/// </summary>
class BoardStatusInteface
{
public:
	// 盤の1辺のマス目の数
	virtual inline const unsigned int GetBoardSide() const = 0;

	// 石の個数
	virtual const int GetPlaceStoneCount(const BoardData::eStone in_stone) const = 0;

	// 指定した石の文字コード
	virtual const char GetStoneCharacterCode(const BoardData::eStone in_stone) const = 0;

	// 指定位置の石種類を取得
	virtual const BoardData::eStone GetPlaceStoneType(int in_x, int in_y) = 0;

	// 指定したマスの開放度取得
	virtual int GetLibertry(int in_x, int in_y) = 0;

	// 石が全て埋まっているか
	virtual bool IsCannotPlacementStone() = 0;

	// 指定した石が盤に置けるかチェック
	virtual bool IsPlacementStone(const BoardData::eStone in_stone) = 0;

	// 指定した石が盤に置ける座標リストを取得
	virtual const std::vector<BoardData::sPoint>& GetPlacementStonePointList(const BoardData::eStone in_stone) = 0;
};

// 石の種類毎にデータを保存するクラス.
// 保存するデータ型を設定できる.
template<typename T> class StoneTypeDataStorage
{
private:
	T _aData[BoardData::eStone_ColorTypeMax];

public:
	T& operator[](int in_stone)
	{
		int idx = in_stone + 1;
		assert(idx < BoardData::eStone_ColorTypeMax);
		assert(idx >= 0);

		return this->_aData[idx];
	}

	const T& operator[](int in_stone) const
	{
		int idx = in_stone + 1;
		assert(idx < BoardData::eStone_ColorTypeMax);
		assert(idx >= 0);

		return this->_aData[idx];
	}

	void operator+= (const StoneTypeDataStorage& in_src)
	{
		for (int i = 0; i < BoardData::eStone_ColorTypeMax; ++i)
		{
			this->_aData[i] += in_src._aData[i];
		}
	}
};

#endif // __BOARD_INTERFACE_H__
