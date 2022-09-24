#pragma once

#include "GUI/GUIMiniHeader.h"
#include "MMD/MMDMiniHeader.h"

#include "GameActor.h"

#include "Component/PMDMeshComponent.h"

namespace App
{
    class PMDActor : public GameActor
    {
    public:
        PMDActor(
            std::shared_ptr<PMD::Factory> in_factory,
            const std::string& in_pmd_filepath,
            const std::string& in_vmd_filepath);

        void SetWorldLocation(DirectX::XMFLOAT3 in_vec) override final;
        DirectX::XMFLOAT3 GetWorldLoaction() override final;

    private:
        std::shared_ptr<Component::PMDMeshCameraComponent> _mesh_comp;
    };
}
