#include "App/BaseCameraActor.h"

#include "GUI/GUIMiniHeader.h"

namespace App
{
	BaseCameraActor::BaseCameraActor(std::shared_ptr<GUI::WindowModelInterface> in_win_model)
	{
		this->_win_model = in_win_model;
	}

	void BaseCameraActor::TickImplement(float in_deltaTimeSecond)
	{
		{
			// カメラ行列作成
			{
				DirectX::XMFLOAT3 target(0.0, this->_eye.y, 0.0);
				DirectX::XMFLOAT3 up(0.0, 1.0f, 0.0);

				this->_view_mat._m = DirectX::XMMatrixLookAtLH(
					DirectX::XMLoadFloat3(&this->_eye),
					DirectX::XMLoadFloat3(&target),
					DirectX::XMLoadFloat3(&up));
			}

			// 射影行列作成
			{
				this->_proj_mat._m = DirectX::XMMatrixPerspectiveFovLH(
					DirectX::XM_PIDIV4,
					this->_win_model->AspectRatio(),
					// カメラが写し始めの値
					1.0f,
					// カメラが写し終わりの値
					100.0f);
			}
		}
	}
}
