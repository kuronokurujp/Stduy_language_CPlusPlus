/*
    ゲーム制御
*/
#pragma once

#include "GUI/GUIMiniHeader.h"

#include "PMD/PMDLoader.h"
#include "PMD/PMDMaterial.h"

namespace App
{
    /// <summary>
    /// ゲームアプリ全体制御
    /// </summary>
    class GameController
    {
    public:
        // シェーダー側に渡すデータ構成
        struct SceneShaderData
        {
            DirectX::XMMATRIX world_mat = DirectX::XMMatrixIdentity();
            DirectX::XMMATRIX view_mat = DirectX::XMMatrixIdentity();
            DirectX::XMMATRIX proj_mat = DirectX::XMMatrixIdentity();
            DirectX::XMFLOAT3 eye = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        };

        GameController(std::shared_ptr<GUI::DirectX12WindowController> in_window_ctrl);

        const bool Start();
        void Update();
        void Render();

        void End();

    private:
        std::shared_ptr<GUI::DirectX12WindowController> _window_ctrl;

        SceneShaderData* _p_scene_shader_param = nullptr;
        PMD::Loader::PMDDataPack _pmd_data_pack;
        std::vector<PMD::Material::Material> _pmd_materials;
        std::vector<PMD::Material::MaterialTexture> _pmd_textures;

        DirectX::XMMATRIX _local_mat = DirectX::XMMatrixIdentity();
        // 3Dの座標変換
        DirectX::XMMATRIX _world_mat = DirectX::XMMatrixIdentity();
        DirectX::XMMATRIX _view_mat = DirectX::XMMatrixIdentity();
        DirectX::XMMATRIX _proj_mat = DirectX::XMMatrixIdentity();
        // 視点
        DirectX::XMFLOAT3 _eye = DirectX::XMFLOAT3(0.0, 10.0f, -15.0);

        UINT32 _material_desc_num = 0;
        UINT32 _material_texture_num = 0;
    };
}
