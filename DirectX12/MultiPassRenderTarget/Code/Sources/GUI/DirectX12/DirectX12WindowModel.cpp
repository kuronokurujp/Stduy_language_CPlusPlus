#include "GUI/DirectX12/DirectX12WindowModel.h"

#include "DirectX12/DirectX12RednerTarget.h"

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
            this->_clear_color = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, cls_clr);
        }
    }

    DirectX12WindowModel::~DirectX12WindowModel()
    {
    }

    void DirectX12WindowModel::EnableUpdate(const bool in_flag)
    {
        this->_update_flag = in_flag;
    }

    /// <summary>
    /// TODO: ポストプロセス用のレンダーターゲットを生成
    /// </summary>
    void DirectX12WindowModel::CreatePostProcessRenderTarget(
        const DirectX12::ComPtr<ID3D12DescriptorHeap> in_src_rtv_desc_heap,
        const DirectX12::ComPtr<ID3D12DescriptorHeap> in_src_dsv_desc_heap,
        const DirectX12::ComPtr<ID3D12Resource> in_src_back_buff)
    {
        if (this->_post_process_render_target)
            return;
        if (!in_src_back_buff.Get())
            return;
        if (!in_src_dsv_desc_heap.Get())
            return;
        if (!in_src_rtv_desc_heap.Get())
            return;

        // TODO: レンダーターゲットを作成してモデルに追加
        this->_post_process_render_target = std::make_shared<DirectX12::RenderTarget>(
            this->_context,
            in_src_rtv_desc_heap,
            in_src_dsv_desc_heap,
            in_src_back_buff,
            this->_clear_color);
    }

    /// <summary>
    /// TODO: ポストプロセス用のレンダーターゲットを破棄
    /// </summary>
    void DirectX12WindowModel::ReleasePostProcessRenderTarget()
    {
        if (!this->_post_process_render_target)
            return;
    }

    const bool DirectX12WindowModel::IsActivePostProcessRenderTarget()
    {
        if (this->_post_process_render_target)
            return true;

        return false;
    }
}
