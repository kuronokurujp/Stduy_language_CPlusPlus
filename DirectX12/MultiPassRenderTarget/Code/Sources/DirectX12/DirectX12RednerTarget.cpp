#include "DirectX12/DirectX12RednerTarget.h"
#include "Geometry/PeraPoly.h"

#include <DirectX12/d3dx12.h>

namespace DirectX12
{
    RenderTarget::RenderTarget(
        std::shared_ptr<DirectX12::Context> in_context,
        ComPtr<ID3D12DescriptorHeap> in_rtv_desc_heap,
        ComPtr<ID3D12DescriptorHeap> in_desc_heap,
        ComPtr<ID3D12Resource> in_back_buff,
        const D3D12_CLEAR_VALUE in_clear_value)
    {
        auto dev = in_context->dev;

        // 大本のRTVのヒープ情報を元にRenderTargetを作成
        auto heap_desc = in_rtv_desc_heap->GetDesc();

        // バックバッファ情報を取得
        auto& bbuff = in_back_buff;
        auto res_desc = bbuff->GetDesc();
        // ターゲット用のリソース作成
        auto heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

        // リソース作成
        {
            auto result = dev->CreateCommittedResource(
                // 割り当てるヒープ指定
                &heap_prop,
                D3D12_HEAP_FLAG_NONE,
                // リソース詳細設定値
                &res_desc,
                // ステートを切り替える予定
                // ステートを切り替える前はテクスチャーとして扱う
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                // クリア色を指定
                &in_clear_value,
                IID_PPV_ARGS(this->_pera_res.ReleaseAndGetAddressOf())
            );
            _ASSERTE(SUCCEEDED(result));
        }

        // RTV用のヒープと割り当てるビューを作る
        D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
        {
            auto local_heap_desc = heap_desc;
            local_heap_desc.NumDescriptors = 1;
            auto result = dev->CreateDescriptorHeap(
                &local_heap_desc,
                IID_PPV_ARGS(this->_pera_rtv_heap.ReleaseAndGetAddressOf())
            );
            // TODO: エラー処理を入れる
            _ASSERTE(SUCCEEDED(result));

            rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            //rtv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            rtv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

            auto handle = this->_pera_rtv_heap->GetCPUDescriptorHandleForHeapStart();
            dev->CreateRenderTargetView(
                this->_pera_res.Get(),
                &rtv_desc,
                handle
            );
            // TODO: エラー処理を入れる
        }

        // SRV用のヒープと割り当てるビューを作る
        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        {
            auto local_heap_desc = heap_desc;
            local_heap_desc.NumDescriptors = 1;
            local_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            local_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            local_heap_desc.NodeMask = 0;

            auto result = dev->CreateDescriptorHeap(
                &local_heap_desc,
                IID_PPV_ARGS(this->_pera_src_heap.ReleaseAndGetAddressOf())
            );
            // TODO: エラー処理を入れる
            _ASSERTE(SUCCEEDED(result));

            srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srv_desc.Format = rtv_desc.Format;
            srv_desc.Texture2D.MipLevels = 1;
            // MEMO: 先に作らないとGPUインスタンシングが中断しているというエラーが起きた
            srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

            auto handle = this->_pera_src_heap->GetCPUDescriptorHandleForHeapStart();
            dev->CreateShaderResourceView(
                this->_pera_res.Get(),
                &srv_desc,
                handle
            );
            // TODO: エラー処理を入れる
        }

        // レンダリングを転写するペラポリ用意
        {
            this->_pera_poly = std::make_shared<Geometry::PeraPoly>(
                "Resources/Shader/Geometry/PeraPoly/PeraVertexShader.hlsl",
                "Resources/Shader/Geometry/PeraPoly/PeraPixelShader.hlsl");
            this->_pera_poly->Create(in_context);
        }
    }

    void RenderTarget::BeginWrite(
        ComPtr<ID3D12GraphicsCommandList> in_cmd_list,
        DirectX12::ComPtr<ID3D12DescriptorHeap> in_dsv_heap,
        const D3D12_CLEAR_VALUE in_clear_value,
        const CD3DX12_VIEWPORT& in_viewport,
        const D3D12_RECT& in_ssissor_rect)

    {
        // リソースをテクスチャからレンダーターゲットに切り替え
        {
            auto transition = CD3DX12_RESOURCE_BARRIER::Transition(
                this->_pera_res.Get(),
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                D3D12_RESOURCE_STATE_RENDER_TARGET
            );

            in_cmd_list->ResourceBarrier(1, &transition);
        }

        // 指定したレンダーターゲット切り替える
        {
            auto rtv_heap_pointer = this->_pera_rtv_heap->GetCPUDescriptorHandleForHeapStart();
            auto dsv_heap_pointer = in_dsv_heap->GetCPUDescriptorHandleForHeapStart();
            in_cmd_list->OMSetRenderTargets(1, &rtv_heap_pointer, false, &dsv_heap_pointer);
            // 画面クリア
            {
                in_cmd_list->ClearRenderTargetView(rtv_heap_pointer, in_clear_value.Color, 0, nullptr);
                in_cmd_list->ClearDepthStencilView(dsv_heap_pointer, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
            }
        }

        // TODO: ここでビューポートやシザーのコマンド設定はだめなの？
        {
            in_cmd_list->RSSetViewports(1, &in_viewport);//ビューポート
            in_cmd_list->RSSetScissorRects(1, &in_ssissor_rect);//シザー(切り抜き)矩形
        }
    }

    void RenderTarget::EndWrite(ComPtr<ID3D12GraphicsCommandList> in_cmd_list)
    {
        // リソースをテクスチャからレンダーターゲットに切り替え
        {
            auto transition = CD3DX12_RESOURCE_BARRIER::Transition(
                this->_pera_res.Get(),
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
            );

            in_cmd_list->ResourceBarrier(1, &transition);
        }
    }

    void RenderTarget::Render(std::shared_ptr<DirectX12::Context> in_context)
    {
        this->_pera_poly->Render(in_context, this->_pera_src_heap);
    }
}
