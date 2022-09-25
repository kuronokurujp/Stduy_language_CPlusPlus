#pragma once

#include "GUI/MVC/Windows/WindowView.h"

#include <vector>

namespace GUI
{
    /// <summary>
    /// DirectX12のウィンドウView
    /// </summary>
    class DirectX12WindowView : public WindowView
    {
    public:
        /// <summary>
        /// ウィンドウオブジェクトを生成
        /// </summary>
        /// <param name="in_p_ctr"></param>
        void CreateObject(WindowControllerInterface* in_p_ctr) override final;

        // 描画開始(描画に必要な前処理をする)
        void BeginRender() override final;

        // 描画終了(描画内容を画面へ反映)
        void EndRender() override final;

    private:
        // ComPtrを利用して解放時にReleaseを呼び出すようにしている
        // DirectX12のAPI全般で利用
        template<typename T>
        using ComPtr = Microsoft::WRL::ComPtr<T>;

        ComPtr<IDXGISwapChain4> _swapchain;
        ComPtr<ID3D12DescriptorHeap> _rtv_heaps;
        ComPtr<ID3D12Resource> _depth_buffer;
        ComPtr<ID3D12DescriptorHeap> _dsv_heap;

        std::vector<ComPtr<ID3D12Resource>> _back_buffers;

        D3D12_RESOURCE_BARRIER _barrier_desc;
    };
}
