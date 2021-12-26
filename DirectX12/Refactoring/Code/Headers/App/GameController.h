/*
    ゲーム制御
*/
#pragma once

#include "GUI/GUIMiniHeader.h"

#include "PMD/PMDLoader.h"
#include "PMD/PMDMaterial.h"
#include "PMD/PMDRendererFactory.h"

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
        void Update();
        void Render();

        void End();

    private:
        std::shared_ptr<GUI::DirectX12WindowController> _window_ctrl;

        std::unique_ptr<PMD::Render::Factory> _pmd_render_factor;

        // TODO: レンダリングテスト用の変数
        DirectX::XMMATRIX _local_mat = DirectX::XMMatrixIdentity();
        // 3Dの座標変換
        DirectX::XMMATRIX _world_mat = DirectX::XMMatrixIdentity();
        DirectX::XMMATRIX _view_mat = DirectX::XMMatrixIdentity();
        DirectX::XMMATRIX _proj_mat = DirectX::XMMatrixIdentity();
        // 視点
        DirectX::XMFLOAT3 _eye = DirectX::XMFLOAT3(0.0, 10.0f, -15.0);
    };
}
