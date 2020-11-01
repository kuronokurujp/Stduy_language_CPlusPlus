// ゲームシーンの制御
#include "Scene/Game/game_scene_controller.h"

#include "System/Interface/console_render_interface.h"
#include "System/Interface/keyboard_interface.h"

#include "Scene/Game/game_scene_model.h"

GameSceneController::GameSceneController(RenderingInterface* in_pRendering, KeyboardInterface* in_pKeyboard)
	: Actor()
{
	this->_Clear();

	this->_pKeyboard = in_pKeyboard;

	// モデル作成
	this->_pModel = new GameSceneModel();
	this->_pModel->Initlize(in_pRendering, in_pKeyboard);
}

void GameSceneController::UpdateActor(float in_deltaTimeSecond)
{
	// ゲームループ
	switch (this->_pModel->GetState())
	{
	case GameSceneModel::eGameState_Boot:
	{
		this->_pModel->GameStart();
		break;
	}
	case GameSceneModel::eGameState_BeginGame:
	{
		this->_pModel->BeginTurn();
		break;
	}
	case GameSceneModel::eGameState_UpdateGame:
	{
		if (!this->_pModel->UpdateGame())
		{
			// すでにボードのマス目に石が埋まっている
			// ゲーム終了かチェック
			this->_pModel->EndTurn();
		}

		break;
	}
	case GameSceneModel::eGameStaet_EndGame:
	{
		// リトライのキーボード入力があれば再度プレイ
		if (this->_pKeyboard->IsConfirmText() == true)
		{
			this->_pModel->GameReset();
		}

		break;
	}
	}

	// アクター更新
	this->_pModel->UpdateActor(in_deltaTimeSecond);
}

void GameSceneController::_Clear()
{
	this->_pModel = NULL;
	this->_pKeyboard = NULL;
}