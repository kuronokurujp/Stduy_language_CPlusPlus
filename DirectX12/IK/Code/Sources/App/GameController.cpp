#include "App/GameController.h"

#include "Common.h"

#include <tchar.h>

namespace App
{
	GameController::GameController(std::shared_ptr<GUI::DirectX12WindowController> in_window_ctrl)
	{
		this->_window_ctrl = in_window_ctrl;

		this->_actor_manager = std::make_unique<Actor::ActorManager>();
		this->_pmd_render_factor = std::make_shared<PMD::Render::Factory>();
	}

	const bool GameController::Start()
	{
		std::shared_ptr<DirectX12::Context> context;
		{
			auto model = this->_window_ctrl->GetModel();
			context = model->Context();
		}
		this->_pmd_render_factor->Initialize(context);

		{
			auto p_pmd_actor = new App::PMDActor(this->_pmd_render_factor, this->_window_ctrl->GetWindowData());
			this->_actor_manager->AddActor(p_pmd_actor);
		}

		return true;
	}

	void GameController::Update()
	{
		// アクター更新
		for (auto actor : this->_actor_manager->GetActors())
			actor->Update(0);
	}

	void GameController::Render()
	{
		for (auto actor : this->_actor_manager->GetActors())
			actor->Render();
	}

	void GameController::End()
	{
		this->_actor_manager->DeleteAllActorsAndMemFree();
		this->_actor_manager.release();

		this->_pmd_render_factor->Terminate();
	}
}