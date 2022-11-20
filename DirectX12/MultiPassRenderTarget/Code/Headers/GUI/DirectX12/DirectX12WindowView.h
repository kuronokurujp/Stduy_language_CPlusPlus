#pragma once

#include "GUI/MVC/Windows/WindowView.h"

#include "DirectX12/DirectX12Core.h"

#include <vector>

namespace GUI
{
    class DirectX12WindowController;

    /// <summary>
    /// DirectX12のウィンドウView
    /// </summary>
    class DirectX12WindowView : public WindowView
    {
    public:
        enum RenderType
        {
            Normal = 0,
            AddPostProcesss,
        };

        friend DirectX12WindowController;

        /// <summary>
        /// ウィンドウオブジェクトを生成
        /// </summary>
        /// <param name="in_p_ctr"></param>
        void CreateObject(WindowControllerInterface* in_p_ctr) override final;

        // 描画開始(描画に必要な前処理をする)
        void BeginRender() override final;

        // 描画終了(描画内容を画面へ反映)
        void EndRender() override final;

        const DirectX12::ComPtr<ID3D12DescriptorHeap> GetRTVDescHeap() const {
            return this->_rtv_desc_heap;
        }

        const DirectX12::ComPtr<ID3D12DescriptorHeap> GetDSVDescHeap() const {
            return this->_dsv_desc_heap;
        }

        const DirectX12::ComPtr<ID3D12Resource> GetBackBuffer() const;

        void SetRenderType(const RenderType in_type) { this->_render_type = in_type; }

    private:
        DirectX12::ComPtr<IDXGISwapChain4> _swapchain;
        DirectX12::ComPtr<ID3D12DescriptorHeap> _rtv_desc_heap;
        DirectX12::ComPtr<ID3D12Resource> _depth_buffer;
        DirectX12::ComPtr<ID3D12DescriptorHeap> _dsv_desc_heap;

        std::vector<DirectX12::ComPtr<ID3D12Resource>> _back_buffers;

        D3D12_RESOURCE_BARRIER _barrier_desc;

        RenderType _render_type = RenderType::Normal;
    };
}
