#include "App/PMDActor.h"

#include <assert.h>

namespace App
{
    PMDActor::PMDActor(std::shared_ptr<PMD::Render::Factory> in_factory, const GUI::WindowCommonData& in_window_data)
    {
        // PMDファイルからレンダリングできるインスタンスを生成
        this->_renderer = in_factory->CreateRenderer(
            "Resources/Model/Miku.pmd",
            "Resources/Shader/PMD/BasicVertexShader.hlsl",
            "Resources/Shader/PMD/BasicPixelShader.hlsl",
            "Resources/Model/Toon/toon%02d.bmp"
        );

        this->_local_mat._m = DirectX::XMMatrixIdentity();
        this->_world_mat._m = DirectX::XMMatrixIdentity();

        {
            // カメラ行列作成
            {
                DirectX::XMFLOAT3 target(0.0, this->_eye.y, 0.0);
                DirectX::XMFLOAT3 up(0.0, 1.0f, 0.0);

                this->_view_mat._m = DirectX::XMMatrixLookAtLH(
                    DirectX::XMLoadFloat3(&this->_eye),
                    DirectX::XMLoadFloat3(&target),
                    DirectX::XMLoadFloat3(&up));
            }

            // 射影行列作成
            {
                this->_proj_mat._m = DirectX::XMMatrixPerspectiveFovLH(
                    DirectX::XM_PIDIV4,
                    static_cast<float>(in_window_data.width) / static_cast<float>(in_window_data.height),
                    // カメラが写し始めの値
                    1.0f,
                    // カメラが写し終わりの値
                    100.0f);
            }
        }

        // VMDファイルからモーションデータ作成
        {
            this->_motion = in_factory->CreateMotion(
                "Resources/Model/VMD/pose.vmd");
        }

        this->_motion->ApplyBoneForRenderer(this->_renderer.get());
    }

    void PMDActor::Render()
    {
        // レンダリング
        this->_renderer->Rendering(
            this->_local_mat._m,
            this->_world_mat._m,
            this->_view_mat._m,
            this->_proj_mat._m,
            this->_eye);
    }
}
