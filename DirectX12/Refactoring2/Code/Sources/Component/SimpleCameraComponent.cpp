#include "Component/SimpleCameraComponent.h"
#include "Component/ActorComponent.h"

#include "Actor/ActorMiniHeader.h"
#include "App/BaseCameraActor.h"

namespace Component
{
    namespace Impl
    {
        static std::vector<App::BaseCameraActor*> FindBaseCameraActors(Actor::Actor* in_p_actor)
        {
            std::vector<App::BaseCameraActor*> find_list;
            if (!in_p_actor)
                return find_list;

            auto actor_component = in_p_actor->GetComponent<ActorComponent*>();
            if (!actor_component)
                return find_list;

            auto actor_manager = actor_component->Get();
            if (!actor_manager)
                return find_list;

            return actor_manager->GetActors<App::BaseCameraActor*>();
        }
    }

    SimpleCameraComponent::SimpleCameraComponent(Actor::Actor* in_pOwner)
        : Component(in_pOwner)
    {
    }

    const DirectX12::XMMATRIX SimpleCameraComponent::GetViewMat() const
    {
        auto camera_actors = Impl::FindBaseCameraActors(this->_pOwner);
        if (camera_actors.size() <= 0)
            return DirectX12::XMMATRIX();

        return camera_actors[0]->GetViewMat();
    }

    const DirectX12::XMMATRIX SimpleCameraComponent::GetProjMat() const
    {
        auto camera_actors = Impl::FindBaseCameraActors(this->_pOwner);
        if (camera_actors.size() <= 0)
            return DirectX12::XMMATRIX();

        return camera_actors[0]->GetProjMat();
    }

    const DirectX::XMFLOAT3 SimpleCameraComponent::GetEye() const
    {
        auto camera_actors = Impl::FindBaseCameraActors(this->_pOwner);
        if (camera_actors.size() <= 0)
            return DirectX::XMFLOAT3();

        return camera_actors[0]->GetEye();
    }
}
