#include "Component/boardRenderComponent.h"

#include "Actor/boardActor.h"
#include "common.h"
#include <stdlib.h>

// ボードを描画するアスキーアート
static const char* s_paBoardAscilArt[18] =
{
	"      a       b       c       d       e       f       g       h    ",
	"  +-------+-------+-------+-------+-------+-------+-------+-------+",
	"1 |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |",
	"  +-------+-------+-------+-------+-------+-------+-------+-------+",
	"2 |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |",
	"  +-------+-------+-------+-------+-------+-------+-------+-------+",
	"3 |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |",
	"  +-------+-------+-------+-------+-------+-------+-------+-------+",
	"4 |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |",
	"  +-------+-------+-------+-------+-------+-------+-------+-------+",
	"5 |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |",
	"  +-------+-------+-------+-------+-------+-------+-------+-------+",
	"6 |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |",
	"  +-------+-------+-------+-------+-------+-------+-------+-------+",
	"7 |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |",
	"  +-------+-------+-------+-------+-------+-------+-------+-------+",
	"8 |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |",
	"  +-------+-------+-------+-------+-------+-------+-------+-------+",
};

BoardRenderComponent::BoardRenderComponent(BoardActor* in_pBoard, ConsoleRenderingInterface* in_pRendering)
	: RenderComponent(reinterpret_cast<Actor*>(in_pBoard), in_pRendering)
{
	this->_Clear();

	this->_pBoard = in_pBoard;
	this->_renderClsCharacteCode = in_pRendering->GetClsCharacteCode();

	// 描画するデータ作成
	{
		// 縦は配列の要素数
		this->_textRectHeight = static_cast<unsigned int>(std::extent<decltype(s_paBoardAscilArt), 0>());
		// 横は文字数
		this->_textRectWidth = static_cast<unsigned int>(strlen(s_paBoardAscilArt[0]));

		// 描画メモリを確保
		unsigned drawRectHalfCharactersItemSize = this->_textRectWidth * this->_textRectHeight;
		// 描画バッファを用意
		this->_pDstBufferDrawRectHalfCharacter = reinterpret_cast<char*>(new char[drawRectHalfCharactersItemSize]);
		for (unsigned int i = 0; i < _textRectHeight; ++i)
		{
			memcpy(&this->_pDstBufferDrawRectHalfCharacter[i * this->_textRectWidth], s_paBoardAscilArt[i], this->_textRectWidth * sizeof(s_paBoardAscilArt[0][0]));
		}
	}
}

BoardRenderComponent::~BoardRenderComponent()
{
	SAFETY_MEM_RELEASE(this->_pDstBufferDrawRectHalfCharacter);
}

/// <summary>
/// 描画.
/// </summary>
/// <returns></returns>
void BoardRenderComponent::Draw(ConsoleRenderingInterface* in_pRendering)
{
	// 盤の描画
	{
		// ボードのマス目から描画バッファを書き換える
		int squaresX = 0;
		int squaresY = 0;
		int arrayStartIndex = 0;
		char srcDrawHalfCharacter = 0;
		char headCharcter = 0;
		for (unsigned int y = 0; y < this->_textRectHeight; ++y)
		{
			// 文字列の先頭にマス目の行数が数字があるか
			headCharcter = s_paBoardAscilArt[y][0];
			if (isdigit(static_cast<unsigned char>(headCharcter)) != 0)
			{
				// マス目の行数を数字0基準にして取得
				squaresY = atoi(&headCharcter) - 1;

				arrayStartIndex = y * this->_textRectWidth;
				// 先頭の文字は書き換える必要がないのでスキップ
				for (unsigned int x = 1; x < this->_textRectWidth; ++x)
				{
					srcDrawHalfCharacter = s_paBoardAscilArt[y][x];
					if (isdigit(static_cast<unsigned char>(srcDrawHalfCharacter)) != 0)
					{
						// ビルド最適化で値がおかしくなるための防止
						squaresX = atoi(&srcDrawHalfCharacter);
						this->_pDstBufferDrawRectHalfCharacter[arrayStartIndex + x] =
							this->GetStoneCharacterCode(this->_pBoard->GetPlaceStoneType(atoi(&srcDrawHalfCharacter), squaresY));
					}
				}
			}
		}

		// 描画バッファに転送
		in_pRendering->FlashRectHalfCharacter(
			this->_pDstBufferDrawRectHalfCharacter,
			0,
			0,
			this->_textRectWidth,
			this->_textRectHeight
		);
	}

	// 盤の状態を表示
	{
		// ステータス描画のアスキーアート
		// 文字列の変更を柔軟にするためstringクラスを利用
		const std::string s_paStatus[] =
		{
			"--------------------------------",
			"          BoardActor Status",
			"--------------------------------",
			std::string("BlackStone(") + this->GetStoneCharacterCode(BoardData::eStone_ColorBlack) + std::string(")"),
			std::string(" Placement Count :") + std::to_string(this->_blackStoneCount),
			"--------------------------------",
			std::string("WhiteStone(") + this->GetStoneCharacterCode(BoardData::eStone_ColorWhite) + std::string(")"),
			std::string(" Placement Count :") + std::to_string(this->_whiteStoneCount),
			"---------------------------------",
			"  / ^---^/",
			" /------/ |",
			" | 8457 | |",
			"  ------",
		};

		for (unsigned int i = 0; i < StaticSingleArrayLength(s_paStatus); ++i)
		{
			// 描画バッファに転送
			in_pRendering->FlashLineHalfCharacter(
				s_paStatus[i].c_str(),
				this->_textRectWidth + 1,
				1 + i);
		}
	}
}

/// <summary>
/// 指定した石から文字コード取得.
/// </summary>
const char BoardRenderComponent::GetStoneCharacterCode(const BoardData::eStone in_stone)
{
	switch (in_stone)
	{
	case BoardData::eStone_ColorWhite: return 'o';
	case BoardData::eStone_ColorBlack: return 'x';
	}

	return this->_renderClsCharacteCode;
}

void BoardRenderComponent::SetStoneCount(const BoardData::eStone in_stone, const unsigned int in_count)
{
	switch (in_stone)
	{
	case BoardData::eStone_ColorBlack: this->_blackStoneCount = in_count; break;
	case BoardData::eStone_ColorWhite: this->_whiteStoneCount = in_count; break;
	}
}

void BoardRenderComponent::_Clear()
{
	this->_textRectHeight = this->_textRectWidth = 0;
	this->_blackStoneCount = this->_whiteStoneCount = 0;

	this->_pDstBufferDrawRectHalfCharacter = NULL;
	this->_renderClsCharacteCode = 0;

	this->_pBoard = NULL;
}