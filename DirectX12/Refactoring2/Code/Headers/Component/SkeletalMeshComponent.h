#pragma once

#include "DirectX12/DirectX12MiniHeader.h"
#include "DirectX12/DirectX12Mesh.h"

#include "Component/Component.h"

#include <memory>

namespace Animation { class BaseAnimation; }

namespace Component
{
    /// <summary>
    /// 骨のあるメッシュを制御するコンポーネント
    /// </summary>
    class SkeltalMeshCameraComponent : public Component
    {
    public:
        SkeltalMeshCameraComponent(
            Actor::Actor* in_pOwner,
            std::shared_ptr<DirectX12::Context> in_context);

        virtual void SetAnimation(std::shared_ptr<Animation::BaseAnimation> in_animation);

        /// <summary>
        /// 毎フレーム更新.
        /// </summary>
        virtual void Update(const float in_deltaTimeSecond) override;

        /// <summary>
        /// 描画
        /// </summary>
        virtual void Render() = 0;

        DirectX12::XMMATRIX* GetWorldMatPtr() { return &this->_world_mat; }

    protected:
        void _CreateMesh(
            std::vector<UINT8>& in_r_vertexs,
            size_t in_vertex_stride_byte_size,
            std::vector<UINT16>& in_r_indices);

    protected:
        std::shared_ptr<Animation::BaseAnimation> _animation;
        std::shared_ptr<DirectX12::Context> _context;
        std::shared_ptr<DirectX12::Mesh> _mesh;

        // 3Dの座標変換
        DirectX12::XMMATRIX _local_mat;
        DirectX12::XMMATRIX _world_mat;
    };
}
