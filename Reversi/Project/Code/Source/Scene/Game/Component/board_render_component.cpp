#include "Scene/Game/Component/board_render_component.h"

#include "Scene/Game/Actor/board_actor.h"
#include "common.h"
#include <stdlib.h>

#ifdef __CUI_GAME__
#else

#include "System/Platform/Win32/win32_frame_renderer.h"

// オブジェクト
#include "Window/Code/Header/Render/PathTracing/Figure/plane.h"
#include "Window/Code/Header/Render/PathTracing/Figure/cylinder.h"

// テクスチャー
#include "Window/Code/Header/Render/PathTracing/Texture/solid_color.h"
#include "Window/Code/Header/Render/PathTracing/Texture/checker_texture.h"

// マテリアル
#include "Window/Code/Header/Render/PathTracing/Material/lambertian.h"

#endif

#ifdef __CUI_GAME__

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

class BoardRenderComponentImpl
{
public:
	BoardRenderComponentImpl(RenderingInterface* in_pRendering)
	{
		this->_render_cls_characte_code = in_pRendering->GetClsCharacteCode();

		// 描画するデータ作成
		{
			// 縦は配列の要素数
			this->_text_rect_height = static_cast<unsigned int>(std::extent<decltype(s_paBoardAscilArt), 0>());
			// 横は文字数
			this->_text_rect_width = static_cast<unsigned int>(strlen(s_paBoardAscilArt[0]));

			// 描画メモリを確保
			unsigned drawRectHalfCharactersItemSize = this->_text_rect_width * this->_text_rect_height;
			// 描画バッファを用意
			this->_p_dst_buffer_draw_rect_half_character = reinterpret_cast<char*>(new char[drawRectHalfCharactersItemSize]);
			for (unsigned int i = 0; i < _text_rect_height; ++i)
			{
				memcpy(&this->_p_dst_buffer_draw_rect_half_character[i * this->_text_rect_width], s_paBoardAscilArt[i], this->_text_rect_width * sizeof(s_paBoardAscilArt[0][0]));
			}
		}
	}

	~BoardRenderComponentImpl()
	{
		SAFETY_MEM_RELEASE(this->_p_dst_buffer_draw_rect_half_character);
	}

	void Draw(RenderingInterface* in_pRendering, BoardActor* in_pBoard)
	{
		assert(in_pBoard != nullptr);

		// 盤の描画
		{
			// ボードのマス目から描画バッファを書き換える
			int squaresX = 0;
			int squaresY = 0;
			int arrayStartIndex = 0;
			char srcDrawHalfCharacter = 0;
			char headCharcter = 0;
			for (unsigned int y = 0; y < this->_text_rect_height; ++y)
			{
				// 文字列の先頭にマス目の行数が数字があるか
				headCharcter = s_paBoardAscilArt[y][0];
				if (isdigit(static_cast<unsigned char>(headCharcter)) != 0)
				{
					// マス目の行数を数字0基準にして取得
					squaresY = atoi(&headCharcter) - 1;

					arrayStartIndex = y * this->_text_rect_width;
					// 先頭の文字は書き換える必要がないのでスキップ
					for (unsigned int x = 1; x < this->_text_rect_width; ++x)
					{
						srcDrawHalfCharacter = s_paBoardAscilArt[y][x];
						if (isdigit(static_cast<unsigned char>(srcDrawHalfCharacter)) != 0)
						{
							// ビルド最適化で値がおかしくなるための防止
							squaresX = atoi(&srcDrawHalfCharacter);
							this->_p_dst_buffer_draw_rect_half_character[arrayStartIndex + x] =
								this->GetStoneCharacterCode(in_pBoard->GetPlaceStoneType(atoi(&srcDrawHalfCharacter), squaresY));
						}
					}
				}
			}

			// 描画バッファに転送
			in_pRendering->FlashRectHalfCharacter(
				this->_p_dst_buffer_draw_rect_half_character,
				0,
				0,
				this->_text_rect_width,
				this->_text_rect_height
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
				std::string(" Placement Count :") + std::to_string(this->_black_stone_count),
				"--------------------------------",
				std::string("WhiteStone(") + this->GetStoneCharacterCode(BoardData::eStone_ColorWhite) + std::string(")"),
				std::string(" Placement Count :") + std::to_string(this->_white_stone_count),
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
					this->_text_rect_width + 1,
					1 + i);
			}
		}
	}

	/// <summary>
	/// 指定した石から文字コード取得.
	/// </summary>
	const char GetStoneCharacterCode(const BoardData::eStone in_stone)
	{
		switch (in_stone)
		{
		case BoardData::eStone_ColorWhite: return 'o';
		case BoardData::eStone_ColorBlack: return 'x';
		}

		return this->_render_cls_characte_code;
	}

private:
	unsigned int _text_rect_height, _text_rect_width;
	unsigned int _black_stone_count, _white_stone_count;

	char* _p_dst_buffer_draw_rect_half_character;
	char _render_cls_characte_code;
};

#else

// GUI用のを作る
// TODO: 作り途中
class BoardRenderComponentImpl
{
public:
	BoardRenderComponentImpl(RenderingInterface* in_pRendering)
	{
		// 版、石を配置
		Win32FrameRenderer* p_win32_renderer = dynamic_cast<Win32FrameRenderer*>(in_pRendering);
		assert(p_win32_renderer != nullptr);

		auto p_raytrace_space = p_win32_renderer->GetRayTraceSpacePtr();

		this->_stone_black_color = make_shared<SolidColor>(Color(0.0, 0.0, 0.0));
		this->_stone_white_color = make_shared<SolidColor>(Color(1.0, 1.0, 1.0));
		this->_stone_gray_color = make_shared<SolidColor>(Color(0.6, 0.6, 0.6));

		// 配置している全オブジェクトのZ値を調整
		double offset_pos_z = -0.2f;

		// 版を作成
		{
			shared_ptr<CheckerTexture> checkerTexture = make_shared<CheckerTexture>(
				make_shared<SolidColor>(Color(0.0, 1.0, 1.0)),
				make_shared<SolidColor>(Color(0.0, 1.0, 0.0)),
				8);

			p_raytrace_space->world.Add(make_shared<Plane>(
				-0.85, 0.85,
				-0.7 + offset_pos_z,
				1.0 + offset_pos_z,
				0.0, make_shared<Lambertian>(checkerTexture)));
		}

		// 版に石を載せた
		this->_stone_count = BoardActor::eBoardSquareCount::eBoardSquaresCount_Side;
		for (int y = 0; y < this->_stone_count; ++y)
		{
			for (int x = 0; x < this->_stone_count; ++x)
			{
				auto pos = Point3(
					-1.0 + 0.25 + (static_cast<double>(x) * 0.215),
					0.0,
					-1.0 + 0.4 + (static_cast<double>(y) * 0.21) + offset_pos_z);

				this->_stones[y * this->_stone_count + x]._cylinder =
					make_shared<Cylinder>(
						pos,
						Math::Vec3(0.0, 0.01, 0.0), 0.1,
						make_shared<Lambertian>(this->_stone_black_color));

				this->_stones[y * this->_stone_count + x]._cylinder->SetHitEnable(true);
				// レイトレース空間にモデルを追加
				p_raytrace_space->world.Add(this->_stones[y * this->_stone_count + x]._cylinder);
			}
		}

		// オブジェクトをノードツリーで配置してレイ衝突検知を最適化
		p_raytrace_space->bvh_node = make_shared<BvhNode>(p_raytrace_space->world, 0, 0);
	}

	/// <summary>
	/// 指定した石から文字コード取得.
	/// </summary>
	const char GetStoneCharacterCode(const BoardData::eStone in_stone)
	{
		switch (in_stone)
		{
		case BoardData::eStone_ColorWhite: return 'o';
		case BoardData::eStone_ColorBlack: return 'x';
		}

		return 0;
	}

	/// <summary>
	/// 描画.
	/// </summary>
	/// <param name="in_pRendering">The in p rendering.</param>
	/// <param name="in_pBoard">The in p board.</param>
	void Draw(RenderingInterface* in_pRendering, BoardActor* in_pBoard)
	{
		// 石のモデルは毎回描画更新している

		// BoardActorとの蜜結合状態であるが、
		// このコンポーネントはBoardActorの利用という前提で作成しているので一旦このままで良い
		assert(in_pBoard != nullptr);

		// 配置した石に応じて色を変えたり、石を表示非表示にする
		_struct_stone_data_* p_stone_data = nullptr;
		for (int y = 0; y < this->_stone_count; ++y)
		{
			for (int x = 0; x < this->_stone_count; ++x)
			{
				auto stone_type = in_pBoard->GetPlaceStoneType(x, y);

				auto stone_single_index = y * this->_stone_count + x;
				p_stone_data = &this->_stones[stone_single_index];
				shared_ptr<Lambertian> p_lambertian = std::dynamic_pointer_cast<Lambertian>(p_stone_data->_cylinder->GetMaterial());
				switch (stone_type)
				{
				case BoardData::eStone::eStone_ColorWhite:
				{
					p_lambertian->SetAlbedo(this->_stone_white_color);
					p_stone_data->_cylinder->SetHitEnable(true);
					break;
				}
				case BoardData::eStone::eStone_None:
				{
					p_stone_data->_cylinder->SetHitEnable(false);
					break;
				}
				case BoardData::eStone::eStone_ColorBlack:
				{
					p_lambertian->SetAlbedo(this->_stone_black_color);
					p_stone_data->_cylinder->SetHitEnable(true);
					break;
				}

				default:
					break;
				}

				// ユーザーが置ける石がどこかを視覚化するため描画
				{
					// マスに置ける石があれば表示
					if (in_pBoard->IsUserPlaceSquares(x, y)) {
						p_lambertian->SetAlbedo(this->_stone_gray_color);
						p_stone_data->_cylinder->SetHitEnable(true);
					}
				}
			}
		}
	}

	void FlashText(
		RenderingInterface* in_pRendering,
		const int in_black_stone_count, const int in_white_stone_count)
	{
		// GUI上でボードの状態が分かるのでテキストであえて表示させる必要はないと判断
		return;

		// 盤の状態を表示
		{
			// ステータス描画のアスキーアート
			// 文字列の変更を柔軟にするためstringクラスを利用
			const std::string s_paStatus[] =
			{
				"--------------------------------",
				std::string("BlackStone(") + this->GetStoneCharacterCode(BoardData::eStone_ColorBlack) + std::string(")"),
				std::string(" Placement Count :") + std::to_string(in_black_stone_count),
				"--------------------------------",
				std::string("WhiteStone(") + this->GetStoneCharacterCode(BoardData::eStone_ColorWhite) + std::string(")"),
				std::string(" Placement Count :") + std::to_string(in_white_stone_count),
				"---------------------------------",
			};

			// テキスト描画バッファに文字を転送
			const unsigned int max = StaticSingleArrayLength(s_paStatus);
			float pos_y = 0.f;
			for (unsigned int i = 0; i < max; ++i)
			{
				// 描画バッファに転送
				in_pRendering->FlashLineHalfCharacter(
					s_paStatus[i].c_str(), 0.f, pos_y);

				pos_y += 0.05f;
			}
		}
	}

	/// <summary>
	/// 指定したモデルハンドルから配置した石のモデルを見つける.
	/// </summary>
	const bool FindStoneModelType(BoardData::sPoint* out_p_point, const unsigned long in_handle)
	{
		assert(out_p_point != NULL);

		_struct_stone_data_* p_stone_data = nullptr;
		for (int y = 0; y < this->_stone_count; ++y)
		{
			int y_index_base = y * this->_stone_count;
			for (int x = 0; x < this->_stone_count; ++x)
			{
				p_stone_data = &this->_stones[y_index_base + x];
				if (p_stone_data->_cylinder->Handle() == in_handle)
				{
					out_p_point->_x = x;
					out_p_point->_y = y;
					return true;
				}
			}
		}

		return false;
	}

private:
	// 石のデータ
	struct _struct_stone_data_
	{
		// 表示するオブジェクト
		shared_ptr<Cylinder> _cylinder;
	};

	// 盤マス目に配置する石
	_struct_stone_data_ _stones[64];

	// 石の色
	shared_ptr<SolidColor> _stone_black_color;
	shared_ptr<SolidColor> _stone_white_color;
	shared_ptr<SolidColor> _stone_gray_color;

	int _stone_count;
};

#endif

BoardRenderComponent::BoardRenderComponent(BoardActor* in_pBoard, RenderingInterface* in_pRendering)
	: RenderComponent(reinterpret_cast<Actor*>(in_pBoard), in_pRendering)
{
	this->_Clear();

	this->_p_board = in_pBoard;
	this->_p_board_impl = new BoardRenderComponentImpl(in_pRendering);
}

BoardRenderComponent::~BoardRenderComponent()
{
	SAFETY_MEM_RELEASE(this->_p_board_impl);
}

/// <summary>
/// 描画.
/// </summary>
/// <returns></returns>
void BoardRenderComponent::Draw(RenderingInterface* in_pRendering)
{
	this->_p_board_impl->Draw(in_pRendering, this->_p_board);

#ifdef __CUI_GAME__
#else
	this->_p_board_impl->FlashText(
		in_pRendering, this->_black_stone_count, this->_white_stone_count);
#endif
}

/// <summary>
/// 指定した石から文字コード取得.
/// </summary>
const char BoardRenderComponent::GetStoneCharacterCode(const BoardData::eStone in_stone)
{
	return this->_p_board_impl->GetStoneCharacterCode(in_stone);
}
#ifdef __CUI_GAME__
#else

const bool BoardRenderComponent::FindStoneModelType(
	BoardData::sPoint* out_p_point, const unsigned long in_handle)
{
	return this->_p_board_impl->FindStoneModelType(out_p_point, in_handle);
}
#endif

void BoardRenderComponent::SetStoneCount(const BoardData::eStone in_stone, const unsigned int in_count)
{
	switch (in_stone)
	{
	case BoardData::eStone_ColorBlack: this->_black_stone_count = in_count; break;
	case BoardData::eStone_ColorWhite: this->_white_stone_count = in_count; break;
	}
}

void BoardRenderComponent::_Clear()
{
	this->_p_board = nullptr;
	this->_p_board_impl = nullptr;
}