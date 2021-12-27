#pragma once

#include "GUI/GUIMiniHeader.h"
#include "Actor/ActorMiniHeader.h"

#include "PMD/PMDRendererFactory.h"

namespace App
{
    class PMDActor : public Actor::Actor
    {
    public:
        PMDActor(std::shared_ptr<PMD::Render::Factory> in_factory, const GUI::WindowCommonData& in_window_data);
        // レンダリングする
        void Render() override final;

    private:
        std::shared_ptr<PMD::Render::Renderer> _renderer;

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
