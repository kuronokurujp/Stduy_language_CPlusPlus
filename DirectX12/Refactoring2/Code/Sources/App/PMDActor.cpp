#include "App/PMDActor.h"
#include "Component/SimpleCameraComponent.h"

#include <assert.h>

namespace App
{
    PMDActor::PMDActor(
        std::shared_ptr<PMD::Render::Factory> in_factory,
        const std::string& in_pmd_filepath,
        const std::string& in_vmd_filepath)
    {
        // 必要コンポーネントを追加
        {
            this->AddComponentMemData(std::make_shared<Component::SimpleCameraComponent>(this));
        }

        // PMDファイルからレンダリングの基本データ作成
        std::shared_ptr<PMD::Render::RenderBaseData> render_data = std::make_shared<PMD::Render::RenderBaseData>();
        in_factory->LoadRenderData(render_data, in_pmd_filepath, "Resources/Model/Toon/toon%02d.bmp");

        // レンダリングコンポーネント作成
        this->_renderer = in_factory->CreateRenderer(
            render_data,
            "Resources/Shader/PMD/BasicVertexShader.hlsl",
            "Resources/Shader/PMD/BasicPixelShader.hlsl");

        this->_local_mat._m = DirectX::XMMatrixIdentity();
        this->_world_mat._m = DirectX::XMMatrixIdentity();

        // VMDファイルからモーションデータ作成
        {
            this->_motion = in_factory->CreateMotion(in_vmd_filepath);
            this->_motion->PlayAnimation();
        }
    }

    PMDActor::~PMDActor()
    {
    }

    void PMDActor::TickImplement(float in_deltaTimeSecond)
    {
        DirectX::XMFLOAT3 up(0.0, 1.0f, 0.0);
        this->_world_mat._m *= DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&up), 0.01f);

        this->_motion->UpdateAnimation(this->_renderer.get(), this->_renderer->GetBaseData()->iks);
    }

    void PMDActor::Render()
    {
        auto simple_camera_component = this->GetComponent<Component::SimpleCameraComponent*>();
        auto view_mat = simple_camera_component->GetViewMat();
        auto proj_mat = simple_camera_component->GetProjMat();
        auto eye_vec = simple_camera_component->GetEye();

        // レンダリング
        this->_renderer->InsertCmdToCmdPipeline(
            this->_local_mat._m,
            this->_world_mat._m,
            view_mat._m,
            proj_mat._m,
            eye_vec);
    }
}
