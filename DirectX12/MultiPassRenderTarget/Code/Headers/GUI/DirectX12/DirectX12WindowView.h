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

        // レンダーターゲットを追加
        void AddRednerTarget();

    private:
        void _ClearRender();

    private:
        DirectX12::ComPtr<IDXGISwapChain4> _swapchain;
        DirectX12::ComPtr<ID3D12DescriptorHeap> _rtv_desc_heap;
        DirectX12::ComPtr<ID3D12Resource> _depth_buffer;
        DirectX12::ComPtr<ID3D12DescriptorHeap> _dsv_desc_heap;

        std::vector<DirectX12::ComPtr<ID3D12Resource>> _back_buffers;
        std::vector<std::shared_ptr<class DirectX12::RenderTarget>> _render_target_list;

        D3D12_RESOURCE_BARRIER _barrier_desc;
    };
}
