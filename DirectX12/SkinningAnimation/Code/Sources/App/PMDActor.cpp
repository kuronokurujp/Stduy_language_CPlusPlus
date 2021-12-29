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
                    DirectX::XM_PIDIV2,
                    static_cast<float>(in_window_data.width) / static_cast<float>(in_window_data.height),
                    // カメラが写し始めの値
                    1.0f,
                    // カメラが写し終わりの値
                    100.0f);
            }
        }

        // VMDファイルからモーションデータ作成
        {
            in_factory->CreateMotion(
                "Resources/Model/VMD/pose.vmd");
        }

        // TEST
        {
            // 左腕を曲げる
            {
                auto node = this->_renderer->_bone_node_table["左腕"];
                auto mat =
                    // 原点へ移動
                    DirectX::XMMatrixTranslation(-node.start_pos.x, -node.start_pos.y, -node.start_pos.z)
                    // 回転
                    * DirectX::XMMatrixRotationZ(DirectX::XM_PIDIV2 * 0.5)
                    // 元の位置に戻す
                    * DirectX::XMMatrixTranslation(node.start_pos.x, node.start_pos.y, node.start_pos.z);
                this->_renderer->_bone_matrices[node.index] = mat;
            }
            // 左ひじを曲げる
            {
                auto node = this->_renderer->_bone_node_table["左ひじ"];
                auto mat =
                    // 原点へ移動
                    DirectX::XMMatrixTranslation(-node.start_pos.x, -node.start_pos.y, -node.start_pos.z)
                    // 回転
                    * DirectX::XMMatrixRotationZ(DirectX::XM_PIDIV2 * 0.5)
                    // 元の位置に戻す
                    * DirectX::XMMatrixTranslation(node.start_pos.x, node.start_pos.y, node.start_pos.z);
                this->_renderer->_bone_matrices[node.index] = mat;
            }

            // ボーン全体を更新
            {
                auto node = this->_renderer->_bone_node_table[PMD::Render::Renderer::_center_bone_name.c_str()];
                this->_renderer->RecursiveMatrixMuliply(&node, DirectX::XMMatrixIdentity());
            }

            std::copy(this->_renderer->_bone_matrices.begin(), this->_renderer->_bone_matrices.end(), this->_renderer->_p_mapped_matrices + 1);
        }
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
