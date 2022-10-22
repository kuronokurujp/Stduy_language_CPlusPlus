#pragma once

#include "DirectX12/DirectX12Core.h"

namespace Geometry { class PeraPoly; }

namespace DirectX12
{
    /// <summary>
    /// DirectX12用のレンダーターゲット
    /// </summary>
    class RenderTarget
    {
    public:
        RenderTarget(
            std::shared_ptr<DirectX12::Context> in_context,
            ComPtr<ID3D12DescriptorHeap> in_rtv_desc_heap,
            ComPtr<ID3D12DescriptorHeap> in_dsv_desc_heap,
            ComPtr<ID3D12Resource> in_back_buff,
            const D3D12_CLEAR_VALUE in_clear_value
        );

        void BeginWrite(
            ComPtr<ID3D12GraphicsCommandList> in_cmd_list,
            DirectX12::ComPtr<ID3D12DescriptorHeap> in_dsv_heap,
            const D3D12_CLEAR_VALUE in_clear_value);
        void EndWrite(ComPtr<ID3D12GraphicsCommandList> in_cmd_list);

        void Render(
            std::shared_ptr<DirectX12::Context> in_context);

    private:
        // ポリゴン張り付けるリソース
        DirectX12::ComPtr<ID3D12Resource> _pera_res;
        // レンダーターゲット用
        DirectX12::ComPtr<ID3D12DescriptorHeap> _pera_rtv_heap;
        // テクスチャ用
        DirectX12::ComPtr<ID3D12DescriptorHeap> _pera_src_heap;

        std::shared_ptr<Geometry::PeraPoly> _pera_poly;
    };
}
