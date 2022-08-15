#pragma once

#include "GUI/GUIMiniHeader.h"
#include "Actor/ActorMiniHeader.h"
#include "MMD/MMDMiniHeader.h"

namespace App
{
    class PMDActor : public Actor::Actor
    {
    public:
        PMDActor(std::shared_ptr<PMD::Render::Factory> in_factory, const GUI::WindowCommonData& in_window_data);

        void UpdateActor(float in_deltaTimeSecond) override final;

        // レンダリングする
        void Render() override final;

    private:
        std::shared_ptr<PMD::Render::Renderer> _renderer;
        std::shared_ptr<PMD::Render::Motion> _motion;
        std::shared_ptr<std::vector<PMD::Loader::PMDIK>> _ik_datas;

        DirectX12::XMMATRIX _local_mat;
        // 3Dの座標変換
        DirectX12::XMMATRIX _world_mat;
        DirectX12::XMMATRIX _view_mat;
        DirectX12::XMMATRIX _proj_mat;
        // 視点
        DirectX::XMFLOAT3 _eye = DirectX::XMFLOAT3(0.0, 10.0f, -30.0);
    };
}
