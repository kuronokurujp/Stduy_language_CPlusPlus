#pragma once

#include "Actor/ActorMiniHeader.h"
#include "MMD/MMDMiniHeader.h"

namespace GUI
{
    class WindowModelInterface;
};

namespace App
{
    /// <summary>
    /// 基本カメラアクター
    /// </summary>
    class BaseCameraActor : public Actor::Actor
    {
    public:
        BaseCameraActor(std::shared_ptr<GUI::WindowModelInterface> in_win_model);
        virtual void TickImplement(float in_deltaTimeSecond) override;

        inline DirectX12::XMMATRIX GetViewMat() const { return this->_view_mat; }
        inline DirectX12::XMMATRIX GetProjMat() const { return this->_proj_mat; }
        inline DirectX::XMFLOAT3 GetEye() const { return this->_eye; }

    protected:
        // カメラの注視点
        DirectX::XMFLOAT3 _target = DirectX::XMFLOAT3(0.0f, this->_eye.y, 0.0f);
        // カメラ位置
        DirectX::XMFLOAT3 _eye = DirectX::XMFLOAT3(0.0f, 10.0f, -30.0f);

        // ビューと射影行列
        DirectX12::XMMATRIX _view_mat;
        DirectX12::XMMATRIX _proj_mat;

        std::shared_ptr<GUI::WindowModelInterface> _win_model;
    };
}
