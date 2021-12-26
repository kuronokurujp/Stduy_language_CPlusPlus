#include "App/GameController.h"

#include "Common.h"
#include "PMD/PMDMaterial.h"

#include <tchar.h>

namespace App
{
    GameController::GameController(std::shared_ptr<GUI::DirectX12WindowController> in_window_ctrl)
    {
        this->_window_ctrl = in_window_ctrl;

        this->_pmd_render_factor = std::make_unique<PMD::Render::Factory>();

        {
            // カメラ行列作成
            {
                DirectX::XMFLOAT3 target(0.0, this->_eye.y, 0.0);
                DirectX::XMFLOAT3 up(0.0, 1.0f, 0.0);

                this->_view_mat = DirectX::XMMatrixLookAtLH(
                    DirectX::XMLoadFloat3(&this->_eye),
                    DirectX::XMLoadFloat3(&target),
                    DirectX::XMLoadFloat3(&up));
            }

            // 射影行列作成
            {
                auto window_data = this->_window_ctrl->GetWindowData();
                this->_proj_mat = DirectX::XMMatrixPerspectiveFovLH(
                    DirectX::XM_PIDIV2,
                    static_cast<float>(window_data.width) / static_cast<float>(window_data.height),
                    // カメラが写し始めの値
                    1.0f,
                    // カメラが写し終わりの値
                    100.0f);
            }
        }
    }

    const bool GameController::Start()
    {
        std::shared_ptr<DirectX12::Context> context;
        {
            auto model = this->_window_ctrl->GetModel();
            context = model->Context();
        }
        this->_pmd_render_factor->Initialize(context);

        // PMDファイルからレンダリングできるインスタンスを生成
        auto mike_renderer = this->_pmd_render_factor->CreateRenderer(
            context,
            "mike",
            "Resources/Model/Miku.pmd",
            "Resources/Shader/PMD/BasicVertexShader.hlsl",
            "Resources/Shader/PMD/BasicPixelShader.hlsl",
            "Resources/Model/Toon/toon%02d.bmp"
        );

        return true;
    }

    void GameController::Update()
    {
    }

    void GameController::Render()
    {
        auto model = this->_window_ctrl->GetModel();
        auto context = model->Context();

        auto mike_render = this->_pmd_render_factor->GetRenderer("mike");
        assert(mike_render.get() != nullptr);

        // レンダリング
        mike_render->Rendering(
            context,
            this->_local_mat,
            this->_world_mat,
            this->_view_mat,
            this->_proj_mat,
            this->_eye);
    }

    void GameController::End()
    {
        this->_pmd_render_factor->Termiate();
    }
}
