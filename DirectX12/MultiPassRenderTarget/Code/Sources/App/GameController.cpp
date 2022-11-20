#include "App/GameController.h"

#include "Common.h"
#include "App/BaseCameraActor.h"

#include <tchar.h>

namespace App
{
    GameController::GameController(std::shared_ptr<GUI::DirectX12WindowController> in_window_ctrl)
    {
        this->_window_ctrl = in_window_ctrl;

        this->_actor_manager = std::make_unique<Actor::ActorManager>();
        this->_pmd_render_factor = std::make_shared<PMD::Factory>();
    }

    const bool GameController::Start()
    {
        std::shared_ptr<DirectX12::Context> context;
        {
            auto model = this->_window_ctrl->GetModel();
            context = model->Context();
        }
        this->_pmd_render_factor->Initialize(context);

        // カメラアクター設定
        {
            auto p_cam_actor = new App::BaseCameraActor(this->_window_ctrl->GetModel());
            this->_actor_manager->AddActor(p_cam_actor);
        }

        // PMDキャラクターのアクター作成
        {
            auto p_pmd_actor = new App::PMDActor(this->_pmd_render_factor, "Resources/Model/Miku.pmd", "Resources/Model/VMD/squat.vmd");
            p_pmd_actor->SetWorldLocation(DirectX::XMFLOAT3(10, 0, 0));
            this->_actor_manager->AddActor(p_pmd_actor);
        }

        // PMDキャラクターのアクター作成
        {
            auto p_pmd_actor = new App::PMDActor(this->_pmd_render_factor, "Resources/Model/Miku.pmd", "Resources/Model/VMD/squat.vmd");
            p_pmd_actor->SetWorldLocation(DirectX::XMFLOAT3(0, 0, 0));
            this->_actor_manager->AddActor(p_pmd_actor);
        }

        // TODO: ポストプロセスのシステム作成
        {
            this->_window_ctrl->ConnectPostProcessSystem();
        }

        return true;
    }

    void GameController::Tick(float in_deltaTimeSecond)
    {
        // アクター更新
        this->_actor_manager->Tick(in_deltaTimeSecond);
    }

    void GameController::Render()
    {
        // ポストプロセス開始
        this->_window_ctrl->BeginPostProcessSystem();
        this->_actor_manager->Render();
        // ポストプロセス終了
        this->_window_ctrl->EndPostProcessSystem();
    }

    void GameController::End()
    {
        this->_actor_manager->DeleteAllActorsAndMemFree();
        this->_actor_manager.release();

        this->_pmd_render_factor->Terminate();

        this->_window_ctrl->BreakPostProcessSystem();
    }
}
