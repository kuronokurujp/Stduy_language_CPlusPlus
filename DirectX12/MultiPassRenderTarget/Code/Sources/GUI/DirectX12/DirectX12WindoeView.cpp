#include "GUI/GUIMiniHeader.h"

#include "DirectX12/d3dx12.h"

#include <assert.h>

namespace GUI
{
    /// <summary>
    /// ウィンドウ作成
    /// </summary>
    /// <param name="in_p_ctr"></param>
    void DirectX12WindowView::CreateObject(WindowControllerInterface* in_p_ctr)
    {
        // 基本ウィンドウ作成
        WindowView::CreateObject(in_p_ctr);

        // ここからはカスタム作成

        // DirectX12機能を利用するのでコンテキスト取得
        auto p_dx12_ctrl = dynamic_cast<DirectX12WindowController*>(this->_p_ctrl);
        assert(p_dx12_ctrl != nullptr);

        std::shared_ptr<DirectX12WindowModel> model = p_dx12_ctrl->GetModel();
        auto context = model->Context();

        // DirectX12での描画機能作成

        // DXのスワップチェーンの生成
        // ダブルバッファリングをするために必要
        // ウィンドウモード前提

        const UINT32 width = model->Width();
        const UINT32 height = model->Height();
        {
            // まずスワップチェイン用の設定データを作る
            DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
            swap_chain_desc.Width = width;
            swap_chain_desc.Height = height;
            swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            swap_chain_desc.Stereo = false;
            // 品質指定
            swap_chain_desc.SampleDesc.Count = 1;
            swap_chain_desc.SampleDesc.Quality = 0;

            // DXGI_USAGE_BACK_BUFFERを利用するとリソースがスワップチェーンかどうかを判定する事が出来るみたい
            // https://docs.microsoft.com/en-us/windows/win32/direct3ddxgi/dxgi-usage
            //swap_chain_desc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
            swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            // ダブルバッファリングするので2
            swap_chain_desc.BufferCount = 2;

            // バックバッファは伸び縮みできる
            swap_chain_desc.Scaling = DXGI_SCALING_STRETCH;

            // フリップ後は破棄
            swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

            // TODO: これなに？
            swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

            // ウィンドウとフルスクリーン切り替え自由に
            swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

            // エラーでも何が原因でエラーになったのかは出力してくれないのだろうか？
            // スワップチェーン生成にはDXGIが必要なのだろうか？
            auto result_swap_chain = context->dxgi_factory->CreateSwapChainForHwnd(
                context->cmd_queue.Get(),
                this->_h_wnd,
                &swap_chain_desc,
                nullptr,
                nullptr,
                (IDXGISwapChain1**)this->_swapchain.ReleaseAndGetAddressOf());

            // 成功するとBufferCount数分、バッファが作成される
            assert(SUCCEEDED(result_swap_chain));
        }

        // ディスクリプタヒープを作成
        // 今回はレンダーターゲットビュー用に作成
        {
            D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
            // 作成するビューを指定
            heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            // GPUが複数ある場合に利用、1つのみなら0でいい
            // 逆に考えれば複数ある場合は選択できるようにするべき
            heap_desc.NodeMask = 0;
            // 作成するディスクリプタの数、ダブルバッファリングなので2にする
            heap_desc.NumDescriptors = 2;
            heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

            auto result = context->dev->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(this->_rtv_desc_heap.ReleaseAndGetAddressOf()));
            assert(SUCCEEDED(result));
        }

        // スワップチェーンとレンダーターゲットメモリを紐づけ
        {
            // スワップチェーンの説明データを取得
            DXGI_SWAP_CHAIN_DESC swap_desc = {};
            auto result = this->_swapchain->GetDesc(&swap_desc);
            assert(SUCCEEDED(result));

            // フロントとバックのレンダーターゲットが必要
            // スワップするバッファ数分確保
            this->_back_buffers.resize(swap_desc.BufferCount);

            D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
            // ガンマ補正ありでレンダリングするように設定している
            rtv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

            // ディスクリプタの先頭ハンドルを取得
            D3D12_CPU_DESCRIPTOR_HANDLE handle = this->_rtv_desc_heap->GetCPUDescriptorHandleForHeapStart();
            auto buff_offset = context->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            for (UINT i = 0; i < swap_desc.BufferCount; ++i)
            {
                // バッファを作成
                result = this->_swapchain->GetBuffer(i, IID_PPV_ARGS(this->_back_buffers[i].ReleaseAndGetAddressOf()));
                assert(SUCCEEDED(result));

                // バッファをレンダーターゲットのバッファだよという意味づけをする
                context->dev->CreateRenderTargetView(
                    this->_back_buffers[i].Get(),
                    &rtv_desc,
                    handle
                );

                // 次に設定するバッファアドレスを計算
                handle.ptr += buff_offset;
            }
        }

        {
            // バッファ作成
            {
                // 深度バッファのリソースを作成
                // 画面全体で利用するのでサイズは画面の縦横
                // 2Dテクスチャ
                auto res_desc = CD3DX12_RESOURCE_DESC::Tex2D(
                    DXGI_FORMAT_D32_FLOAT,
                    width,
                    height,
                    1,
                    0,
                    1,
                    0,
                    D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
                );

                // ヒープのプロパティ内容
                auto heap_property = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT, 0, 0);

                // バッファをクリアする内容
                D3D12_CLEAR_VALUE depth_clear_value = {};
                {
                    depth_clear_value.DepthStencil.Depth = 1.0f;
                    depth_clear_value.Format = DXGI_FORMAT_D32_FLOAT;
                }

                // リソースバッファを作成
                auto result = context->dev->CreateCommittedResource(
                    &heap_property,
                    D3D12_HEAP_FLAG_NONE,
                    &res_desc,
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    // 作成するバッファのクリアする内容
                    &depth_clear_value,
                    IID_PPV_ARGS(this->_depth_buffer.ReleaseAndGetAddressOf())
                );
                assert(SUCCEEDED(result));
            }

            // 深度バッファ用のディスクリプタヒープを作る
            {
                D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
                // 作成するビューを指定
                heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
                heap_desc.NumDescriptors = 1;

                auto result = context->dev->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(this->_dsv_desc_heap.ReleaseAndGetAddressOf()));
                assert(SUCCEEDED(result));
            }

            // 深度バッファと関連付けしたディスクリプタビューを作る
            {
                D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
                dsv_desc.Format = DXGI_FORMAT_D32_FLOAT;
                dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                dsv_desc.Flags = D3D12_DSV_FLAG_NONE;

                context->dev->CreateDepthStencilView(
                    // 深度バッファ
                    this->_depth_buffer.Get(),
                    &dsv_desc,
                    // ビュー対応させるディスクリプタヒープのアドレス
                    this->_dsv_desc_heap->GetCPUDescriptorHandleForHeapStart()
                );
            }
        }
    }

    // 描画開始(描画に必要な前処理をする)
    void DirectX12WindowView::BeginRender()
    {
        // バックバッファのレンダーターゲットインデックスを取得
        auto bb_idx = this->_swapchain->GetCurrentBackBufferIndex();

        // バックバッファをレンダーターゲットとして使用通知のバリアコマンド追加
        // EndRenderメソッドで利用するので壊さないように注意
        this->_barrier_desc = CD3DX12_RESOURCE_BARRIER::Transition(
            this->_back_buffers[bb_idx].Get(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        );

        auto p_dx12_ctrl = dynamic_cast<DirectX12WindowController*>(this->_p_ctrl);
        auto model = p_dx12_ctrl->GetModel();
        auto context = model->Context();
        context->cmd_list->ResourceBarrier(1, &this->_barrier_desc);

        // レンダーターゲットビューを設定コマンド追加
        // 参照するディスクリプタのポインターを利用してレンダーターゲットビューを設定
        auto rtv_h = this->_rtv_desc_heap->GetCPUDescriptorHandleForHeapStart();
        auto dsv_h = this->_dsv_desc_heap->GetCPUDescriptorHandleForHeapStart();
        {
            rtv_h.ptr += bb_idx * context->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

            context->cmd_list->OMSetRenderTargets(
                1,
                &rtv_h,
                true,
                // 深度バッファを作成したヒープハンドルを設定
                &dsv_h);
        }

        // 深度バッファをクリア
        // 書き込まれた深度値をクリアしないと深度判定がうまくいかない
        {
            context->cmd_list->ClearDepthStencilView(dsv_h, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
        }

        // 設定したレンダーターゲットを色クリア
        {
            // 画面全体をクリアする色
            const auto& clear_color = model->GetClearClear();
            context->cmd_list->ClearRenderTargetView(rtv_h, clear_color.Color, 0, nullptr);
        }

        // ビュー領域とシザー領域を設定
        context->cmd_list->RSSetViewports(1, &model->GetViewPort());
        context->cmd_list->RSSetScissorRects(1, &model->GetScissorRect());
    }

    // 描画終了(描画内容を画面へ反映)
    void DirectX12WindowView::EndRender()
    {
        auto p_dx12_ctrl = dynamic_cast<DirectX12WindowController*>(this->_p_ctrl);
        auto model = p_dx12_ctrl->GetModel();

        auto context = model->Context();

        // バックバッファをフロントバッファにする
        {
            // リソース指定をPresentにする
            this->_barrier_desc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            this->_barrier_desc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
            context->cmd_list->ResourceBarrier(1, &this->_barrier_desc);
        }

        // コマンドを閉じてからコマンドを実行を一緒にやっている
        // フェンスをやっていてコマンドが終了するまで待機もしている
        DirectX12::ExecuteAndCloseCmd(context);

        // バッファを入れ替え
        // 第1引数を1にする事でVSYNCを有効にする
        // FPSはDisplayの設定に依存するので注意
        this->_swapchain->Present(1, 0);

        // コマンドリストのリセット
        context->cmd_allocator->Reset();
        // コマンドのクローズ状態を解除
        context->cmd_list->Reset(context->cmd_allocator.Get(), nullptr);
    }
}
