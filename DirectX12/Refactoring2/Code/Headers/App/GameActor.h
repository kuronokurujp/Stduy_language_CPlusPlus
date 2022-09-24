#pragma once

#include "Actor/ActorMiniHeader.h"

#include <DirectXMath.h>

namespace App
{
    class GameActor : public Actor::Actor
    {
    public:
        virtual void SetWorldLocation(DirectX::XMFLOAT3 in_vec) {}
        virtual DirectX::XMFLOAT3 GetWorldLoaction() { return DirectX::XMFLOAT3(); }

        void Render() override final;
    };
}
