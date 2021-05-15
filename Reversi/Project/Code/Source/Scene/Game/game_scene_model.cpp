// ゲームモデル
#include "Scene/Game/game_scene_model.h"

#include "common.h"

#include "System/Interface/console_render_interface.h"
#include "System/Interface/keyboard_interface.h"

#include "Actor/actor_manager.h"
#include "Actor/terminal_actor.h"

#include "Scene/Game/Actor/board_actor.h"
#include "Scene/Game/Actor/user_actor.h"

#include "Scene/Game/Component/player_component.h"
#include "Scene/Game/Component/enemy_component.h"

GameSceneModel::GameSceneModel()
{
	this->_p_actor_manger = new ActorManager();
}

GameSceneModel::~GameSceneModel()
{
	SAFETY_MEM_RELEASE(this->_p_terminal);
	SAFETY_MEM_RELEASE(this->_p_board);

	// ゲームプレイヤーのユーザーを破棄
	{
		for (unsigned int i = 0; i < StaticSingleArrayLength(this->_user_info_group.aUsers); ++i)
		{
			SAFETY_MEM_RELEASE(this->_user_info_group.aUsers[i].pUserActor);
		}
	}

	this->_p_actor_manger->DeleteAllActorsAndMemFree();
	SAFETY_MEM_RELEASE(this->_p_actor_manger);
}

void GameSceneModel::Initlize(RenderingInterface* in_pRendering, KeyboardInterface* in_pKeyboard)
{
	assert(in_pKeyboard != NULL);
	assert(in_pRendering != NULL);

	this->_p_keyboard = in_pKeyboard;
	// ターミナルアクター作成
	{
		this->_p_terminal = new TerminalActor(in_pRendering);
		this->_p_actor_manger->AddActorMemData(this->_p_terminal);
	}

	// 盤のアクター作成
	{
		this->_p_board = new BoardActor(in_pRendering);
		this->_p_actor_manger->AddActorMemData(this->_p_board);
	}

	// 白と黒の指し手を生成
	{
		// ユーザーの設定データ宣言・定義
		struct UserInfoSetup
		{
			BoardData::eStone stone;
		};

		static UserInfoSetup aSetupDatas[] =
		{
			{BoardData::eStone_ColorBlack},
			{BoardData::eStone_ColorWhite},
		};
		assert(StaticSingleArrayLength(aSetupDatas) == StaticSingleArrayLength(this->_user_info_group.aUsers));

		// 指し手の生成
		for (unsigned int i = 0; i < StaticSingleArrayLength(this->_user_info_group.aUsers); ++i)
		{
			auto setupData = aSetupDatas[i];
			auto pSetupUserInfo = &this->_user_info_group.aUsers[i];

			pSetupUserInfo->stone = aSetupDatas[i].stone;
			pSetupUserInfo->pUserActor = new UserActor(setupData.stone, in_pRendering);
		}
	}
}

void GameSceneModel::GameStart()
{
	assert(this->_game_state == eGameState_Boot);

	// ユーザー設定
	{
		unsigned int userMax = StaticSingleArrayLength(this->_user_info_group.aUsers);
		for (int unsigned i = 0; i < userMax; ++i)
		{
			auto pUserInfo = &this->_user_info_group.aUsers[i];
			auto pUser = this->_user_info_group.aUsers[i].pUserActor;

			this->_p_actor_manger->RemoveActor(pUser);

			{
				pUser->RemoveComponentClassNameAndMemFree<EnemyComponent>();
				pUser->RemoveComponentClassNameAndMemFree<PlayerComponent>();
			}

			this->_p_actor_manger->AddActorMemData(pUser);
			pUser->SetState(Actor::eState_Pause);

			unsigned int otherUserIndex = (i + 1) % userMax;
			auto pOtherActor = this->_user_info_group.aUsers[otherUserIndex].pUserActor;

			// 黒か白の打ち手の種類で応じ設定を変更
			switch (pUserInfo->stone)
			{
			case BoardData::eStone_ColorBlack:
			{
				// 黒石の打ち手にプレイヤーのコンポーネント設定
				new PlayerComponent(pUser, this->_p_keyboard);

				break;
			}

			case BoardData::eStone_ColorWhite:
			{
				// 白色の打ち手に敵のコンポーネント設定
				new EnemyComponent(pUser, pOtherActor, this->_p_board, this->_p_board);
				break;
			}
			default:
				break;
			}
		}
	}

	// ゲーム開始
	this->_turn = 0;

	this->_p_terminal->Cls();
	this->_p_board->Reset();

	this->_p_now_turn_playuser = &this->_user_info_group.aUsers[this->_turn];
	this->_p_now_turn_playuser->pUserActor->StartTurn(this->_p_board, this->_p_board);

	this->_game_state = eGameState_BeginGame;
}

void GameSceneModel::GameReset()
{
	this->_game_state = eGameState_Boot;
}

void GameSceneModel::BeginTurn()
{
	assert(this->_game_state == eGameState_BeginGame);

	this->_game_state = eGameState_UpdateGame;
}

bool GameSceneModel::UpdateGame()
{
	assert(this->_game_state == eGameState_UpdateGame);

	// 指し手の状態をターミナルに表示
	this->_p_terminal->WriteFullAreaText(this->_p_now_turn_playuser->pUserActor->ToText());

	// 指し手がゲーム中
	if (this->_p_now_turn_playuser->pUserActor->IsPlaying() == false)
	{
		// すでにボードのマス目に石が埋まっている
		// ゲーム終了かチェック
		if (this->_p_board->IsCannotPlacementStone() == true)
		{
			return false;
		}
		else
		{
			this->_p_terminal->Cls();

			// ターン切り替え
			++this->_turn;

			// ターン毎にユーザーを切り替え
			unsigned int nowUserIndex = this->_turn % StaticSingleArrayLength(this->_user_info_group.aUsers);

			this->_p_now_turn_playuser = &this->_user_info_group.aUsers[nowUserIndex];
			this->_p_now_turn_playuser->pUserActor->StartTurn(this->_p_board, this->_p_board);
		}
	}

	return true;
}

void GameSceneModel::EndTurn()
{
	assert(this->_game_state == eGameState_UpdateGame);

	// すでにボードのマス目に石が埋まっている
	// ゲーム終了かチェック
	if (this->_p_board->IsCannotPlacementStone() == true)
	{
		// ゲーム結果
		const int placeWhiteStone = this->_p_board->GetPlaceStoneCount(BoardData::eStone_ColorWhite);
		const int placeBlackStone = this->_p_board->GetPlaceStoneCount(BoardData::eStone_ColorBlack);

		std::string winnerText("winner: ");
		if (placeBlackStone > placeWhiteStone)
		{
			// 黒石の打ち手の勝利
			winnerText += "black stone";
		}
		else if (placeBlackStone < placeWhiteStone)
		{
			// 白石の打ち手の勝利
			winnerText += "white stone";
		}
		else
		{
			// 引き分け
			winnerText = "draw!!!";
		}

		// 画面表示を作る
		const std::string  aResultTerminalText[] =
		{
			"Game Result!!",
			// 勝者の表記
			winnerText,
			"-- Retry InputKey => r",
			std::string("- InputText: ") + this->_p_keyboard->GetConfirmInputText(),
		};

		for (unsigned int i = 0; i < StaticSingleArrayLength(aResultTerminalText); ++i)
		{
			this->_p_terminal->WriteLineText(i, aResultTerminalText[i].c_str());
		}

		this->_game_state = eGameStaet_EndGame;
	}
	else
	{
		this->_p_terminal->Cls();

		// ターン切り替え
		++this->_turn;

		// ターン毎にユーザーを切り替え
		unsigned int nowUserIndex = this->_turn % StaticSingleArrayLength(this->_user_info_group.aUsers);

		this->_p_now_turn_playuser = &this->_user_info_group.aUsers[nowUserIndex];
		this->_p_now_turn_playuser->pUserActor->StartTurn(this->_p_board, this->_p_board);
	}
}

void GameSceneModel::UpdateActor(const float in_deltaTimeSecond)
{
	// アクター更新
	{
		for (auto actor : this->_p_actor_manger->GetActors())
		{
			actor->Update(in_deltaTimeSecond);
		}
	}
}

void GameSceneModel::NoticeTouchEvent(const InputComponent::eTouchEvent t, std::vector<InputComponent::_touch_event_data_>& d)
{
	// イベント通知コンポーネントがあれば通知
	for (auto actor : this->_p_actor_manger->GetActors())
	{
		auto p_input_component = actor->GetComponent<InputComponent*>();
		if (p_input_component != NULL)
			p_input_component->OnTouchEvent(t, d);
	}
}

void GameSceneModel::_Clear()
{
	this->_p_actor_manger = NULL;
	this->_p_keyboard = NULL;
	this->_p_now_turn_playuser = NULL;
	this->_p_board = NULL;

	this->_p_terminal = NULL;
	this->_game_state = eGameState_Boot;
	this->_turn = 0;
}