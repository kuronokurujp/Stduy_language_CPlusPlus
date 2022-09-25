#pragma once

#include "Animation/BaseAnimation.h"
#include "DirectX12/DirectX12Math.h"

#include "MMD/PMDLoader.h"
#include "MMD/VMDLoader.h"

#include "Component/PMDMeshComponent.h"

#include <memory>
#include <map>
#include <string>
#include <Windows.h>

namespace VMD {
    namespace Loader { struct VMDDataPack; }
}

namespace Animation
{
    class VMDAnimation : public BaseAnimation
    {
    public:
        /// <summary>
        /// モーションのキーフレーム
        /// </summary>
        struct MotionKeyFrame
        {
            UINT32 frame_no = 0;
            DirectX::XMVECTOR quaternion = DirectX::XMVectorZero();
            DirectX::XMFLOAT3 offset;
            DirectX::XMFLOAT2 p1;
            DirectX::XMFLOAT2 p2;

            MotionKeyFrame(
                const UINT32 in_frame_no,
                DirectX::XMVECTOR& in_r_que,
                const DirectX::XMFLOAT3& in_offset,
                DirectX::XMFLOAT2 in_p1,
                DirectX::XMFLOAT2 in_p2)
                :
                frame_no(in_frame_no),
                quaternion(in_r_que),
                offset(in_offset),
                p1(in_p1),
                p2(in_p2)
            {}
        };

        /// <summary>
        /// VMDデータパックからアニメデータ構築
        /// </summary>
        /// <param name="in_r_data_pack"></param>
        VMDAnimation(const VMD::Loader::VMDDataPack& in_r_data_pack);

        void PlayAnimation() override final;
        void UpdateAnimation(Component::SkeltalMeshCameraComponent* in_p_mesh_comp) override final;

    private:
        void _UpdateAnimation(Component::PMDMeshCameraComponent* in_p_comp);

        /// <summary>
        /// 2次元を2点を使ったベジェ曲線計算
        /// </summary>
        const float _CalcBezierByTwo2DPoint(
            const float in_t, const DirectX::XMFLOAT2& in_r_p1, const DirectX::XMFLOAT2& in_r_p2, const UINT32 in_n);

        /// <summary>
        /// IK解決処理
        /// </summary>
        void _IKSolve(
            Component::PMDMeshCameraComponent* in_p_comp,
            const std::shared_ptr<Component::PMDMeshCameraComponent::RenderBaseData> in_p_render_base_data, const UINT32 in_now_frame);

        /// <summary>
        /// CCD-IKによるボーン方向を解決
        /// </summary>
        /// <param name="in_r_ik"></param>
        void _SolveCCDIK(
            Component::PMDMeshCameraComponent* in_p_comp,
            const std::shared_ptr<Component::PMDMeshCameraComponent::RenderBaseData> in_p_render_base_data, const PMD::Loader::PMDIK& in_r_ik);

        /// <summary>
        /// 余弦定理によるボーン方向を解決
        /// </summary>
        /// <param name="in_r_ik"></param>
        void _SolveCosineIK(
            const std::shared_ptr<Component::PMDMeshCameraComponent::RenderBaseData> in_p_render_base_data, const PMD::Loader::PMDIK& in_r_ik);

        /// <summary>
        /// LockAt行列によるボーン方向を解決
        /// </summary>
        /// <param name="in_r_ik"></param>
        void _SolveLockIK(
            const std::shared_ptr<Component::PMDMeshCameraComponent::RenderBaseData> in_p_render_base_data, const PMD::Loader::PMDIK& in_r_ik);

    private:
        std::map<std::string, std::vector<MotionKeyFrame>> _motion_key_frames;
        std::vector<VMD::Loader::VMDIkEnable> _ik_enables;
        DWORD _start_time = 0;
        UINT32 _motion_duration = 0;
    };
}
