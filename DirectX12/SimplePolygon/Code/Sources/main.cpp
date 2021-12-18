#include <Windows.h>
#include <tchar.h>
#include <assert.h>
#include <vector>

// テスト・サンプル用のファイル
#include "polygon_sample.hpp"
#include "shader_test.hpp"

#ifdef _DEBUG
#include <iostream>
#endif

// DXをインクルード
// これらのファイル群はどの環境にも入っているわけじゃないよね？
// ue4はエンジンプロジェクトの中にDXのファイル群が含まれていた
#include <d3d12.h>
#include <dxgi1_6.h>

// DXのライブラリを含める
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace std;

/// <summary>
/// コンソールに文字列描画(デバッグ版)
/// </summary>
/// <param name="in_p_format"></param>
/// <param name=""></param>
void DebugOutputFormatString(const char* in_p_format, ...)
{
#ifdef _DEBUG

    va_list valist;
    // 変数名をva_listとするとva_list型と混在してコンパイルエラーになるので注意
    va_start(valist, in_p_format);
    vprintf(in_p_format, valist);
    va_end(valist);

#endif
}

/// <summary>
/// レイヤーデバッグ機能を有効にする(デバッグ版のみ)
/// </summary>
void EnableDebugLayer()
{
    // デバッグレイヤーのインターフェイス取得
    ID3D12Debug* p_debug_layer = nullptr;
    auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&p_debug_layer));
    assert(result == S_OK);

    // レイヤーを有効
    p_debug_layer->EnableDebugLayer();
    // インターフェイスはもう不要なので解放
    p_debug_layer->Release();
}

LRESULT WindowProcedure(HWND in_hwnd, UINT in_msg, WPARAM in_wparam, LPARAM in_lparam)
{
    if (in_msg == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(in_hwnd, in_msg, in_wparam, in_lparam);
}

// デバッグと非デバッグとでエントリーポイントを分けている
// これはデバッグの場合はコンソールウィンドウを出してデバッグ情報を出力するため
#ifdef _DEBUG
int main()
{
    EnableDebugLayer();

#else

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#endif
    DebugOutputFormatString("DirectX12 Sample.");

    // ウィンドウクラスに登録するデータを生成
    WNDCLASSEX w = {};
    {
        w.cbSize = sizeof(w);
        w.lpfnWndProc = (WNDPROC)WindowProcedure;
        w.lpszClassName = _T("DX12Sample");
        w.hInstance = GetModuleHandle(nullptr);
        RegisterClassEx(&w);
    }

    // ウィンドウのサイズ決める
    LONG window_width = 640;
    LONG window_height = 480;
    RECT wrc = { 0, 0, window_width, window_height };
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

    // ウィンドウデータ作成
    HWND hwnd = CreateWindow(
        w.lpszClassName,
        _T(""),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        wrc.right - wrc.left,
        wrc.bottom - wrc.top,
        nullptr,
        nullptr,
        w.hInstance,
        nullptr
    );

    // DXGIはハードウェアやドライブを直接制御ができる
    // 主な用途としては
    // DXの描画インフラストラクチャーを生成して画面フリップや描画デバイスを制御
    IDXGIFactory6* p_dxgi_factory = nullptr;
    {
        // 末尾の数字は何？
#ifdef _DEBUG
        auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&p_dxgi_factory));
        assert(result == S_OK);
#else
        auto result = CreateDXGIFactory1(IID_PPV_ARGS(&p_dxgi_factory));
        assert(result == S_OK);
#endif
    }

    // DXのデバイス作成
    ID3D12Device* p_dev = nullptr;
    {
        D3D_FEATURE_LEVEL feture_level;
        // 失敗は考えない
        {
            D3D_FEATURE_LEVEL levels[] = {
                D3D_FEATURE_LEVEL_12_1,
                D3D_FEATURE_LEVEL_12_0,
                D3D_FEATURE_LEVEL_11_1,
                D3D_FEATURE_LEVEL_11_0,
                D3D_FEATURE_LEVEL_10_1,
                D3D_FEATURE_LEVEL_10_0,
                D3D_FEATURE_LEVEL_9_3,
                D3D_FEATURE_LEVEL_9_2,
                D3D_FEATURE_LEVEL_9_1,
            };
            for (auto level : levels)
            {
                // デバイス指定はnullptrで自動指定にしている
                if (D3D12CreateDevice(nullptr, level, IID_PPV_ARGS(&p_dev)) == S_OK)
                {
                    feture_level = level;
                    break;
                }
            }

            assert(p_dev != nullptr);
        }
    }

    // DXのコマンドリストを生成
    ID3D12CommandAllocator* p_cmd_allocator = nullptr;
    ID3D12GraphicsCommandList* p_cmd_list = nullptr;
    {
        // GPUに命令するためのコマンドリストのデータ
        // メモリサイズは動的に変わるのだろうか？
        auto result_command_allocator = p_dev->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(&p_cmd_allocator));
        assert(result_command_allocator == S_OK);

        // GPUに命令するためのメソッドインターフェイスを作成
        auto result_command_list = p_dev->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            p_cmd_allocator,
            nullptr,
            IID_PPV_ARGS(&p_cmd_list));
        assert(result_command_list == S_OK);
    }

    // DXのコマンドリストを実行するコマンドを生成してコマンドキューを生成
    ID3D12CommandQueue* p_cmd_queue = nullptr;
    {
        D3D12_COMMAND_QUEUE_DESC cmd_queue_desc = {};

        // タイムアウトなし
        cmd_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        // アダプターを１つのみ使うなら0
        cmd_queue_desc.NodeMask = 0;
        // プライオリティーの設定を通常
        cmd_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        // コマンドリストを合わせる
        cmd_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        auto result_command_queue = p_dev->CreateCommandQueue(
            &cmd_queue_desc,
            IID_PPV_ARGS(&p_cmd_queue));
        assert(result_command_queue == S_OK);
    }

    // DXのスワップチェーンの生成
    IDXGISwapChain4* p_swapchain = nullptr;

    // ダブルバッファリングをするために必要
    // ウィンドウモード前提
    {
        // まずスワップチェイン用の設定データを作る
        DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
        swap_chain_desc.Width = window_width;
        swap_chain_desc.Height = window_height;
        swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swap_chain_desc.Stereo = false;
        // マルチサンプルの指定
        swap_chain_desc.SampleDesc.Count = 1;
        swap_chain_desc.SampleDesc.Quality = 0;

        swap_chain_desc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
        // ダブルバッファリングするので2
        swap_chain_desc.BufferCount = 2;

        // バックバッファは伸び縮みできる
        swap_chain_desc.Scaling = DXGI_SCALING_STRETCH;

        // フリップ後は破棄
        swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

        // これなに？
        swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

        // ウィンドウとフルスクリーン切り替え自由に
        swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        // エラーでも何が原因でエラーになったのかは出力してくれないのだろうか？
        // スワップチェーン生成にはDXGIが必要なのだろうか？
        auto result_swap_chain = p_dxgi_factory->CreateSwapChainForHwnd(
            p_cmd_queue,
            hwnd,
            &swap_chain_desc,
            nullptr,
            nullptr,
            (IDXGISwapChain1**)&p_swapchain);
        // 成功するとBufferCount数分、バッファが作成される
        assert(SUCCEEDED(result_swap_chain));
    }

    // ディスクリプタヒープを作成
    // 今回はレンダーターゲットビュー用に作成
    ID3D12DescriptorHeap* p_rtv_heaps = nullptr;
    {
        D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
        // 作成するビューを指定
        heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        // GPUが複数ある場合に利用、1つのみなら0でいい
        // 逆に考えれば複数ある場合は選択できるようにするべき
        heap_desc.NodeMask = 0;
        // 作成するディスクリプタの数、ダブルバッファリングなので２にする
        heap_desc.NumDescriptors = 2;
        heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        auto result = p_dev->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&p_rtv_heaps));
        assert(SUCCEEDED(result));
    }

    // スワップチェーンとメモリを紐づけ
    std::vector<ID3D12Resource*> back_buffers;
    {
        // スワップチェーンの説明データを取得してバッファを作成
        DXGI_SWAP_CHAIN_DESC swap_desc = {};
        auto result = p_swapchain->GetDesc(&swap_desc);
        assert(SUCCEEDED(result));

        // スワップチェーンのバッファに応じたバックバッファを作成
        back_buffers.resize(swap_desc.BufferCount);

        // ディスクリプタの先頭ハンドルを取得
        D3D12_CPU_DESCRIPTOR_HANDLE handle = p_rtv_heaps->GetCPUDescriptorHandleForHeapStart();
        for (UINT i = 0; i < swap_desc.BufferCount; ++i)
        {
            result = p_swapchain->GetBuffer(i, IID_PPV_ARGS(&back_buffers[i]));
            assert(SUCCEEDED(result));

            // レンダーをディスクリプタを指定して作成
            p_dev->CreateRenderTargetView(
                back_buffers[i],
                nullptr,
                handle
            );

            // ハンドルが参照するアドレスを更新して、次に参照するディスクリプタのアドレス値を作る
            handle.ptr += p_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        }
    }

    // フェンスオブジェクトを作成
    ID3D12Fence* p_fence = nullptr;
    UINT64 fence_val = 0;
    {
        auto result = p_dev->CreateFence(fence_val, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&p_fence));
        assert(SUCCEEDED(result));
    }

    // 頂点バッファビュー作成
    D3D12_VERTEX_BUFFER_VIEW vb_view = {};
    {
        // 頂点情報
        static DirectX::XMFLOAT3 s_vertices[] = {
            // 左下
            {-0.4f, -0.7f, 0.0f},
            // 左上
            {-0.4f, 0.7f, 0.0f},
            // 右下
            {0.4f, -0.7f, 0.0f},
            // 右上
            {0.4f, 0.7f, 0.0f},
        };
        // 頂点バッファを生成
        ID3D12Resource* p_vert_buff = DirectX::CreateResource(p_dev, sizeof(s_vertices));
        assert(p_vert_buff != nullptr);

        // 作った頂点バッファに情報をコピーする
        DirectX::CopyVerticesData(p_vert_buff, std::begin(s_vertices), std::end(s_vertices));

        // 頂点バッファ構成を出力
        DirectX::OutputVerticesBufferView(&vb_view, p_vert_buff, sizeof(s_vertices), sizeof(s_vertices[0]));
    }

    // 頂点のインデックスバッファビューを作成
    D3D12_INDEX_BUFFER_VIEW ib_view = {};
    {
        unsigned short a_indices[] =
        {
            0, 1, 2,
            2, 1, 3
        };

        // インデックスバッファを作成
        ID3D12Resource* p_idx_buff = nullptr;
        {
            p_idx_buff = DirectX::CreateResource(p_dev, sizeof(a_indices));
            unsigned short* p_mapped_idx = nullptr;
            p_idx_buff->Map(0, nullptr, (void**)&p_mapped_idx);
            std::copy(std::begin(a_indices), std::end(a_indices), p_mapped_idx);
            p_idx_buff->Unmap(0, nullptr);
        }

        ib_view.BufferLocation = p_idx_buff->GetGPUVirtualAddress();
        ib_view.Format = DXGI_FORMAT_R16_UINT;
        ib_view.SizeInBytes = sizeof(a_indices);
    }

    // シェーダーファイルロード
    ID3D10Blob* p_vs_blob = nullptr;
    ID3D10Blob* p_ps_blob = nullptr;
    {
        std::string error;
        // 頂点シェーダーをロード
        if (!DirectX::LoadVSShaderFileSample(&p_vs_blob, &error))
        {
            DebugOutputFormatString(error.c_str());
        }

        // ピクセルシェーダーをロード
        if (!DirectX::LoadPSShaderFileSample(&p_ps_blob, &error))
        {
            DebugOutputFormatString(error.c_str());
        }
    }

    // 頂点レイアウト定義
    D3D12_INPUT_ELEMENT_DESC input_layout[] = {
        {
            "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },
    };

    // ルートシグネチャー(ディスクリプタテーブルをまとめたもの)を作る
    ID3D12RootSignature* p_root_sig = nullptr;
    {
        ID3DBlob* p_root_sig_blob = nullptr;

        D3D12_ROOT_SIGNATURE_DESC root_sig_desc = {};
        root_sig_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        ID3DBlob* p_error_blob = nullptr;
        auto result = D3D12SerializeRootSignature(
            &root_sig_desc,
            // 1_0としているが環境によっては1_1でないとだめなのもあるので本来なら対応しているバージョンを探して設定する必要がある
            D3D_ROOT_SIGNATURE_VERSION_1_0,
            &p_root_sig_blob,
            // 一旦エラーが出ててもなにもしない
            &p_error_blob
        );
        // 結果を見て成功しているかチェック
        assert(SUCCEEDED(result));

        result = p_dev->CreateRootSignature(
            0,
            p_root_sig_blob->GetBufferPointer(),
            p_root_sig_blob->GetBufferSize(),
            IID_PPV_ARGS(&p_root_sig)
        );
        // シグネチャー用のBLOBは不要になったので解放
        p_root_sig_blob->Release();
    }

    // グラフィックスパイプラインステートを構築
    ID3D12PipelineState* p_pipeline_state = nullptr;
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};

        // ルートシグネチャーを設定
        gpipeline.pRootSignature = p_root_sig;

        // シェーダー設定
        {
            gpipeline.VS.pShaderBytecode = p_vs_blob->GetBufferPointer();
            gpipeline.VS.BytecodeLength = p_vs_blob->GetBufferSize();

            gpipeline.PS.pShaderBytecode = p_ps_blob->GetBufferPointer();
            gpipeline.PS.BytecodeLength = p_ps_blob->GetBufferSize();
        }

        // サンプルマスクはデフォルトにする
        gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

        // ラスタライズの設定
        {
            // アンチエイリアスは無効にする
            gpipeline.RasterizerState.MultisampleEnable = false;

            // カリングしない
            gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

            // 中身を塗りつぶす
            gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;

            // 深度方向のクリッピングは有効にする
            gpipeline.RasterizerState.DepthClipEnable = true;
        }

        // ブレンドの設定
        {
            gpipeline.BlendState.AlphaToCoverageEnable = false;
            gpipeline.BlendState.IndependentBlendEnable = false;

            D3D12_RENDER_TARGET_BLEND_DESC render_target_blend_desc = {};
            render_target_blend_desc.BlendEnable = false;
            render_target_blend_desc.LogicOpEnable = false;
            render_target_blend_desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

            gpipeline.BlendState.RenderTarget[0] = render_target_blend_desc;
        }

        // 入力レイアウトの設定
        {
            gpipeline.InputLayout.pInputElementDescs = input_layout;
            gpipeline.InputLayout.NumElements = _countof(input_layout);
        }

        // トライアングルストリップでの頂点の扱いを設定
        // 頂点の切り離しを無効
        gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

        // プリミティブ型を設定
        // 三角形にする
        gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        // レンダーターゲット設定
        {
            // ターゲットの数が１なので１に
            gpipeline.NumRenderTargets = 1;
            // 一つのターゲットに色のフォーマット指定
            gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        }

        // アンチエイリアシング設定
        {
            gpipeline.SampleDesc.Count = 1;
            gpipeline.SampleDesc.Quality = 0;
        }

        auto result = p_dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&p_pipeline_state));
        assert(SUCCEEDED(result));
    }

    // ビューポート作成
    D3D12_VIEWPORT viewport = {};
    {
        viewport.Width = window_width;
        viewport.Height = window_height;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.MaxDepth = 1.0f;
        viewport.MinDepth = 0.0f;
    }

    // シザー矩形作成
    // ビューポートいっぱい表示させる
    D3D12_RECT scissorrect = {};
    {
        scissorrect.top = 0;
        scissorrect.left = 0;
        scissorrect.right = scissorrect.left + window_width;
        scissorrect.bottom = scissorrect.top + window_height;
    }

    // ウィンドウ表示
    ShowWindow(hwnd, SW_SHOW);

    MSG msg = {};
    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT)
            break;

        // バックバッファのレンダーターゲットインデックスを取得
        auto bb_idx = p_swapchain->GetCurrentBackBufferIndex();

        // バックバッファをレンダーターゲットとして使用通知のバリアコマンド追加
        D3D12_RESOURCE_BARRIER barrier_desc = {};
        {
            barrier_desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier_desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier_desc.Transition.pResource = back_buffers[bb_idx];
            barrier_desc.Transition.Subresource = 0;

            // 利用するリソースをレンダーターゲットとして指定
            barrier_desc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
            barrier_desc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

            p_cmd_list->ResourceBarrier(1, &barrier_desc);
        }

        // レンダーターゲットビューを設定コマンド追加
        // 参照するディスクリプタのポインターを利用してレンダーターゲットビューを設定
        auto rtvH = p_rtv_heaps->GetCPUDescriptorHandleForHeapStart();
        {
            rtvH.ptr += bb_idx * p_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            p_cmd_list->OMSetRenderTargets(
                1,
                &rtvH,
                true,
                nullptr);
        }

        // 設定したレンダーターゲットをクリア
        float clear_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        p_cmd_list->ClearRenderTargetView(rtvH, clear_color, 0, nullptr);

        p_cmd_list->SetPipelineState(p_pipeline_state);
        p_cmd_list->SetGraphicsRootSignature(p_root_sig);
        p_cmd_list->RSSetViewports(1, &viewport);
        p_cmd_list->RSSetScissorRects(1, &scissorrect);

        p_cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //p_cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

        // 一度に設定できるインデックスバッファビューは一つのみ
        p_cmd_list->IASetIndexBuffer(&ib_view);
        // 頂点バッファビューを設定
        p_cmd_list->IASetVertexBuffers(0, 1, &vb_view);

        // 描画に使用する頂点数を指定
        //p_cmd_list->DrawInstanced(4, 1, 0, 0);
        // インデックスバッファビューを使って描画
        // インデックス数を指定
        p_cmd_list->DrawIndexedInstanced(6, 1, 0, 0, 0);

        // バックバッファをフロントバッファにする
        {
            // リソース指定をPresentにする
            barrier_desc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            barrier_desc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
            p_cmd_list->ResourceBarrier(1, &barrier_desc);
        }

        // これ以上コマンドをためれないようにする
        p_cmd_list->Close();

        // コマンドリストの実行
        ID3D12CommandList* p_cmd_lists[] = { p_cmd_list };
        p_cmd_queue->ExecuteCommandLists(1, p_cmd_lists);

        // コマンド実行のフェンスを通知
        p_cmd_queue->Signal(p_fence, ++fence_val);
        {
            if (p_fence->GetCompletedValue() != fence_val)
            {
                // コマンドリストの命令が全て終了の待ち状態を作る
                auto event = CreateEvent(nullptr, false, false, nullptr);
                p_fence->SetEventOnCompletion(fence_val, event);

                // イベントが実行されるまで待ち
                WaitForSingleObject(event, INFINITE);

                CloseHandle(event);
            }
        }

        // コマンドリストのリセット
        p_cmd_allocator->Reset();
        // コマンドのクローズ状態を解除
        p_cmd_list->Reset(p_cmd_allocator, nullptr);

        // バッファを入れ替え
        p_swapchain->Present(1, 0);
    }

    UnregisterClass(w.lpszClassName, w.hInstance);

    return 0;
}