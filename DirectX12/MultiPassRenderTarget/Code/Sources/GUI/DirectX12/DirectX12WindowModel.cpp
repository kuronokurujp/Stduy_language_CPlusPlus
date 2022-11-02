﻿#include "GUI/DirectX12/DirectX12WindowModel.h"

#include <assert.h>

namespace GUI
{
    DirectX12WindowModel::DirectX12WindowModel(
        std::shared_ptr<DirectX12::Context> in_context,
        const UINT32 in_width,
        const UINT32 in_height)
    {
        this->_context = in_context;
        this->_update_flag = true;

        // ビューポート作成
        this->_viewport = CD3DX12_VIEWPORT(
            0.0f,
            0.0f,
            static_cast<FLOAT>(in_width),
            static_cast<FLOAT>(in_height));

        // シザー矩形作成
        // ビューポートいっぱい表示させる
        {
            this->_scissor_rect.top = 0;
            this->_scissor_rect.left = 0;
            this->_scissor_rect.right = this->_scissor_rect.left + in_width;
            this->_scissor_rect.bottom = this->_scissor_rect.top + in_height;
        }

        // レンダリングクリア値を作成
        {
            float cls_clr[4] = { 1.0, 1.0, 1.0, 1.0 };
            this->_clear_color = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, cls_clr);
        }
    }

    DirectX12WindowModel::~DirectX12WindowModel()
    {
    }

    void DirectX12WindowModel::EnableUpdate(const bool in_flag)
    {
        this->_update_flag = in_flag;
    }
}
