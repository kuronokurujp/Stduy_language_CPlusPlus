#include "game_controller.h"

#include <stdio.h>
#include <assert.h>

#include "common.h"

#include "Actor/actor_manager.h"
#include "Actor/terminal_actor.h"

#include "Scene/Game/game_scene_controller.h"

#include "System/Interface/console_render_interface.h"
#include "System/Interface/keyboard_interface.h"

GameController::GameController(RenderingInterface* in_pRendering, KeyboardInterface* in_pKeyboard)
{
	this->_Clear();

	this->_pRendering = in_pRendering;
	this->_pKeyboard = in_pKeyboard;

	this->_pActorManger = new ActorManager();

	// ゲームシーンへ遷移
	auto pGameSceneController = new GameSceneController(in_pRendering, in_pKeyboard);
	this->_pActorManger->AddActorMemData(pGameSceneController);
}

GameController::~GameController()
{
	this->_pActorManger->DeleteAllActorsAndMemFree();
	SAFETY_MEM_RELEASE(this->_pActorManger);
}

void GameController::Update(const float in_deltaTimeSecond)
{
	// アクター更新
	{
		for (auto actor : this->_pActorManger->GetActors())
		{
			actor->Update(in_deltaTimeSecond);
		}
	}
}

void GameController::_Clear()
{
	this->_pActorManger = NULL;
	this->_pRendering = NULL;
	this->_pKeyboard = NULL;
}