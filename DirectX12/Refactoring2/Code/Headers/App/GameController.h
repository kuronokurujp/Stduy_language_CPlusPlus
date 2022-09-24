/*
    ゲーム制御
*/
#pragma once

#include "GUI/GUIMiniHeader.h"
#include "Actor/ActorMiniHeader.h"

#include "PMDActor.h"

namespace App
{
    /// <summary>
    /// ゲームアプリ全体制御
    /// </summary>
    class GameController
    {
    public:
        GameController(std::shared_ptr<GUI::DirectX12WindowController> in_window_ctrl);

        const bool Start();
        void Tick(float in_deltaTimeSecond);
        void Render();

        void End();

    private:
        std::shared_ptr<GUI::DirectX12WindowController> _window_ctrl;
        std::unique_ptr<Actor::ActorManager> _actor_manager;

        std::shared_ptr<PMD::Factory> _pmd_render_factor;
    };
}
