#include "App/PMDActor.h"
#include "Component/SimpleCameraComponent.h"

#include <assert.h>

namespace App
{
    PMDActor::PMDActor(
        std::shared_ptr<PMD::Factory> in_factory,
        const std::string& in_pmd_filepath,
        const std::string& in_vmd_filepath)
    {
        // PMDファイルからレンダリングの基本データ作成
        std::shared_ptr<Component::PMDMeshCameraComponent::RenderBaseData> render_data = std::make_shared<Component::PMDMeshCameraComponent::RenderBaseData>();
        in_factory->LoadRenderData(render_data, in_pmd_filepath, "Resources/Model/Toon/toon%02d.bmp");

        // 必要コンポーネントを追加
        {
            this->AddComponentMemData(std::make_shared<Component::SimpleCameraComponent>(this));

            // メッシュコンポーネント作成
            this->_mesh_comp = in_factory->CreateMeshComponent(
                this,
                render_data,
                "Resources/Shader/PMD/BasicVertexShader.hlsl",
                "Resources/Shader/PMD/BasicPixelShader.hlsl");
            this->AddComponentMemData(this->_mesh_comp);

            // VMDファイルからアニメーションを作成してメッシュに設定
            auto anim = in_factory->CreateAnimation(in_vmd_filepath);
            this->_mesh_comp->SetAnimation(anim);
            anim->PlayAnimation();
        }
    }

    void PMDActor::SetWorldLocation(DirectX::XMFLOAT3 in_vec)
    {
        auto p_mat = this->_mesh_comp->GetWorldMatPtr();

        DirectX::XMFLOAT4 v4 = DirectX::XMFLOAT4(in_vec.x, in_vec.y, in_vec.z, 1.0f);
        p_mat->_m.r[3] = DirectX::XMLoadFloat4(&v4);
    }

    DirectX::XMFLOAT3 PMDActor::GetWorldLoaction()
    {
        auto p_mat = this->_mesh_comp->GetWorldMatPtr();

        DirectX::XMFLOAT4 v4;
        DirectX::XMStoreFloat4(&v4, p_mat->_m.r[3]);

        return DirectX::XMFLOAT3(v4.x, v4.y, v4.z);
    }
}
