#include "gameWorld.h"

#include <stdio.h>
#include <assert.h>

#include "common.h"

#include "Actor/actorManager.h"
#include "Actor/terminalActor.h"
#include "Actor/boardActor.h"
#include "Actor/userActor.h"

#include "Interface/consoleRenderInterface.h"
#include "Interface/keyboardInterface.h"

#include "Component/playerComponent.h"
#include "Component/enemyComponent.h"

GameWorld::GameWorld(ConsoleRenderingInterface* in_pRendering, KeyboardInterface* in_pKeyboard)
{
	this->_Clear();

	this->_pRendering = in_pRendering;
	this->_pKeyboard = in_pKeyboard;

	this->_pActorManger = new ActorManager();

	// ターミナルアクター作成
	{
		this->_pTerminal = new TerminalActor(in_pRendering);
		this->_pActorManger->AddActorMemData(this->_pTerminal);
	}

	// 盤のアクター作成
	{
		this->_pBoard = new BoardActor(in_pRendering);
		this->_pActorManger->AddActorMemData(this->_pBoard);
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

GameWorld::~GameWorld()
{
	SAFETY_MEM_RELEASE(this->_pTerminal);
	SAFETY_MEM_RELEASE(this->_pBoard);

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

void GameWorld::Update(const float in_deltaTimeSecond)
{
	// ゲームループ
	switch (this->_gameState)
	{
	case eGameState_Boot:
	{
		// 初回ブート
		this->_gameState = eGameState_BeginGame;
		break;
	}
	case eGameState_BeginGame:
	{
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
					new EnemyComponent(pUser, pOtherActor, this->_pBoard, this->_pBoard);
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
		this->_pBoard->Reset();

		this->_pNowTurnPlayUser = &this->_userInfoGroup.aUsers[this->_turn];
		this->_pNowTurnPlayUser->pUserActor->StartTurn(this->_pBoard, this->_pBoard);

		this->_gameState = eGameState_UpdateGame;
		break;
	}
	case eGameState_UpdateGame:
	{
		// 指し手の状態をターミナルに表示
		this->_pTerminal->WriteFullAreaText(this->_pNowTurnPlayUser->pUserActor->ToText());

		// 指し手がゲーム中
		if (this->_pNowTurnPlayUser->pUserActor->IsPlaying() == false)
		{
			// すでにボードのマス目に石が埋まっている
			// ゲーム終了かチェック
			if (this->_pBoard->IsCannotPlacementStone() == true)
			{
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
				this->_pNowTurnPlayUser->pUserActor->StartTurn(this->_pBoard, this->_pBoard);
			}
		}

		break;
	}
	case eGameStaet_EndGame:
	{
		// ゲーム結果
		const int placeWhiteStone = this->_pBoard->GetPlaceStoneCount(BoardData::eStone_ColorWhite);
		const int placeBlackStone = this->_pBoard->GetPlaceStoneCount(BoardData::eStone_ColorBlack);

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

		// リトライのキーボード入力があれば再度プレイ
		if (this->_pKeyboard->IsConfirmText() == true)
		{
			std::string inputText(this->_pKeyboard->GetConfirmEnterText());
			if (inputText.find('r') == 0)
			{
				this->_gameState = eGameState_BeginGame;
			}
		}

		break;
	}
	}

	// アクター更新
	{
		for (auto actor : this->_pActorManger->GetActors())
		{
			actor->Update(in_deltaTimeSecond);
		}
	}
}

void GameWorld::_Clear()
{
	this->_pActorManger = NULL;
	this->_pNowTurnPlayUser = NULL;
	this->_pBoard = NULL;
	this->_pTerminal = NULL;
	this->_pRendering = NULL;
	this->_pKeyboard = NULL;

	this->_gameState = eGameState_Boot;

	this->_turn = 0;
}