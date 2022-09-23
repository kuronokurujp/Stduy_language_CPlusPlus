#pragma once

#include "GUI/GUIMiniHeader.h"
#include "Actor/ActorMiniHeader.h"
#include "MMD/MMDMiniHeader.h"

namespace App
{
    class PMDActor : public Actor::Actor
    {
    public:
        PMDActor(
            std::shared_ptr<PMD::Render::Factory> in_factory,
            const std::string& in_pmd_filepath,
            const std::string& in_vmd_filepath);
        virtual ~PMDActor();

        void TickImplement(float in_deltaTimeSecond) override final;

        // レンダリングする
        void Render() override final;

    private:
        std::shared_ptr<PMD::Render::Renderer> _renderer;
        std::shared_ptr<PMD::Render::Motion> _motion;

        // 3Dの座標変換
        DirectX12::XMMATRIX _local_mat;
        DirectX12::XMMATRIX _world_mat;
    };
}
