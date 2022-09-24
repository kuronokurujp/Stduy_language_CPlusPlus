#include "Component/SkeletalMeshComponent.h"

#include "Animation/BaseAnimation.h"

#include "Common.h"

namespace Component
{
    SkeltalMeshCameraComponent::SkeltalMeshCameraComponent(
        Actor::Actor* in_pOwner,
        std::shared_ptr<DirectX12::Context> in_context)
        : Component(in_pOwner)
    {
        this->_context = in_context;

        this->_local_mat._m = DirectX::XMMatrixIdentity();
        this->_world_mat._m = DirectX::XMMatrixIdentity();
    }

    void SkeltalMeshCameraComponent::SetAnimation(std::shared_ptr<Animation::BaseAnimation> in_animation)
    {
        this->_animation = in_animation;
    }

    void SkeltalMeshCameraComponent::Update(const float in_deltaTimeSecond)
    {
        this->_animation->UpdateAnimation(this);
    }

    void SkeltalMeshCameraComponent::_CreateMesh(std::vector<UINT8>& in_r_vertexs, size_t in_vertex_stride_byte_size, std::vector<UINT16>& in_r_indices)
    {
        // メッシュ作成
        this->_mesh = std::make_shared<DirectX12::Mesh>("vs_buff" + Common::CreateGUIDString(), "idx_buff" + Common::CreateGUIDString());
        this->_mesh->Create(this->_context, in_r_vertexs, in_vertex_stride_byte_size, in_r_indices);
    }
}
