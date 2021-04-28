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
	this->_pActorManger = new ActorManager();
}

GameSceneModel::~GameSceneModel()
{
	SAFETY_MEM_RELEASE(this->_pTerminal);
	SAFETY_MEM_RELEASE(this->_p_board);

	// ゲームプレイヤーのユーザーを破棄
	{
		for (unsigned int i = 0; i < StaticSingleArrayLength(this->_userInfoGroup.aUsers); ++i)
		{
			SAFETY_MEM_RELEASE(this->_userInfoGroup.aUsers[i].pUserActor);
		}
	}

	this->_pActorManger->DeleteAllActorsAndMemFree();
	SAFETY_MEM_RELEASE(this->_pActorManger);
}

void GameSceneModel::Initlize(RenderingInterface* in_pRendering, KeyboardInterface* in_pKeyboard)
{
	assert(in_pKeyboard != NULL);
	assert(in_pRendering != NULL);

	this->_pKeyboard = in_pKeyboard;
	// ターミナルアクター作成
	{
		this->_pTerminal = new TerminalActor(in_pRendering);
		this->_pActorManger->AddActorMemData(this->_pTerminal);
	}

	// 盤のアクター作成
	{
		this->_p_board = new BoardActor(in_pRendering);
		this->_pActorManger->AddActorMemData(this->_p_board);
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
		assert(StaticSingleArrayLength(aSetupDatas) == StaticSingleArrayLength(this->_userInfoGroup.aUsers));

		// 指し手の生成
		for (unsigned int i = 0; i < StaticSingleArrayLength(this->_userInfoGroup.aUsers); ++i)
		{
			auto setupData = aSetupDatas[i];
			auto pSetupUserInfo = &this->_userInfoGroup.aUsers[i];

			pSetupUserInfo->stone = aSetupDatas[i].stone;
			pSetupUserInfo->pUserActor = new UserActor(setupData.stone, in_pRendering);
		}
	}
}

void GameSceneModel::GameStart()
{
	assert(this->_gameState == eGameState_Boot);

	// ユーザー設定
	{
		unsigned int userMax = StaticSingleArrayLength(this->_userInfoGroup.aUsers);
		for (int unsigned i = 0; i < userMax; ++i)
		{
			auto pUserInfo = &this->_userInfoGroup.aUsers[i];
			auto pUser = this->_userInfoGroup.aUsers[i].pUserActor;

			this->_pActorManger->RemoveActor(pUser);

			{
				pUser->RemoveComponentClassNameAndMemFree<EnemyComponent>();
				pUser->RemoveComponentClassNameAndMemFree<PlayerComponent>();
			}

			this->_pActorManger->AddActorMemData(pUser);
			pUser->SetState(Actor::eState_Pause);

			unsigned int otherUserIndex = (i + 1) % userMax;
			auto pOtherActor = this->_userInfoGroup.aUsers[otherUserIndex].pUserActor;

			// 黒か白の打ち手の種類で応じ設定を変更
			switch (pUserInfo->stone)
			{
			case BoardData::eStone_ColorBlack:
			{
				// 黒石の打ち手にプレイヤーのコンポーネント設定
				new PlayerComponent(pUser, this->_pKeyboard);

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

	this->_pTerminal->Cls();
	this->_p_board->Reset();

	this->_pNowTurnPlayUser = &this->_userInfoGroup.aUsers[this->_turn];
	this->_pNowTurnPlayUser->pUserActor->StartTurn(this->_p_board, this->_p_board);

	this->_gameState = eGameState_BeginGame;
}

void GameSceneModel::GameReset()
{
	this->_gameState = eGameState_Boot;
}

void GameSceneModel::BeginTurn()
{
	assert(this->_gameState == eGameState_BeginGame);

	this->_gameState = eGameState_UpdateGame;
}

bool GameSceneModel::UpdateGame()
{
	assert(this->_gameState == eGameState_UpdateGame);

	// 指し手の状態をターミナルに表示
	this->_pTerminal->WriteFullAreaText(this->_pNowTurnPlayUser->pUserActor->ToText());

	// 指し手がゲーム中
	if (this->_pNowTurnPlayUser->pUserActor->IsPlaying() == false)
	{
		// すでにボードのマス目に石が埋まっている
		// ゲーム終了かチェック
		if (this->_p_board->IsCannotPlacementStone() == true)
		{
			return false;
		}
		else
		{
			this->_pTerminal->Cls();

			// ターン切り替え
			++this->_turn;

			// ターン毎にユーザーを切り替え
			unsigned int nowUserIndex = this->_turn % StaticSingleArrayLength(this->_userInfoGroup.aUsers);

			this->_pNowTurnPlayUser = &this->_userInfoGroup.aUsers[nowUserIndex];
			this->_pNowTurnPlayUser->pUserActor->StartTurn(this->_p_board, this->_p_board);
		}
	}

	return true;
}

void GameSceneModel::EndTurn()
{
	assert(this->_gameState == eGameState_UpdateGame);

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
			std::string("- InputText: ") + this->_pKeyboard->GetConfirmInputText(),
		};

		for (unsigned int i = 0; i < StaticSingleArrayLength(aResultTerminalText); ++i)
		{
			this->_pTerminal->WriteLineText(i, aResultTerminalText[i].c_str());
		}

		this->_gameState = eGameStaet_EndGame;
	}
	else
	{
		this->_pTerminal->Cls();

		// ターン切り替え
		++this->_turn;

		// ターン毎にユーザーを切り替え
		unsigned int nowUserIndex = this->_turn % StaticSingleArrayLength(this->_userInfoGroup.aUsers);

		this->_pNowTurnPlayUser = &this->_userInfoGroup.aUsers[nowUserIndex];
		this->_pNowTurnPlayUser->pUserActor->StartTurn(this->_p_board, this->_p_board);
	}
}

void GameSceneModel::UpdateActor(const float in_deltaTimeSecond)
{
	// アクター更新
	{
		for (auto actor : this->_pActorManger->GetActors())
		{
			actor->Update(in_deltaTimeSecond);
		}
	}
}

void GameSceneModel::_Clear()
{
	this->_pActorManger = NULL;
	this->_pKeyboard = NULL;
	this->_pNowTurnPlayUser = NULL;
	this->_p_board = NULL;

	this->_pTerminal = NULL;
	this->_gameState = eGameState_Boot;
	this->_turn = 0;
}