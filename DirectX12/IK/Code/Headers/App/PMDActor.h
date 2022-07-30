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
        /// <summary>
        /// IK解決処理
        /// TODO: 作成中
        /// </summary>
        void _IKSolve();

        /// <summary>
        /// CCD-IKによるボーン方向を解決
        /// TODO: 作成中
        /// </summary>
        /// <param name="in_r_ik"></param>
        void _SolveCCDIK(const PMD::Loader::PMDIK& in_r_ik) {}
        /// <summary>
        /// 余弦定理によるボーン方向を解決
        /// TODO: 作成中
        /// </summary>
        /// <param name="in_r_ik"></param>
        void _SolveCosineIK(const PMD::Loader::PMDIK& in_r_ik) {}
        /// <summary>
        /// LockAt行列によるボーン方向を解決
        /// TODO: 作成中
        /// </summary>
        /// <param name="in_r_ik"></param>
        void _SolveLockIK(const PMD::Loader::PMDIK& in_r_ik) {}
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
