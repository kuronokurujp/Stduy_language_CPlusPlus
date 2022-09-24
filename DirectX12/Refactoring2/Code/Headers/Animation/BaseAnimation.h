#pragma once

#include <memory>

namespace Component { class SkeltalMeshCameraComponent; }

namespace Animation
{
    class BaseAnimation
    {
    public:
        virtual void PlayAnimation() = 0;
        virtual void UpdateAnimation(Component::SkeltalMeshCameraComponent* in_p_mesh_comp) = 0;
    };
}
