#pragma once

#include "Component/Component.h"

#include "DirectX12/DirectX12MiniHeader.h"

namespace Component
{
    class SimpleCameraComponent : public Component
    {
    public:
        SimpleCameraComponent(Actor::Actor* in_pOwner);

        const DirectX12::XMMATRIX GetViewMat() const;
        const DirectX12::XMMATRIX GetProjMat() const;
        const DirectX::XMFLOAT3 GetEye() const;
    };
}
