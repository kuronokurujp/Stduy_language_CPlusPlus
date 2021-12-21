/*
    DirextX12のPMDファイルを利用したモデル描画
*/
#include <Windows.h>
#include <tchar.h>
#include <assert.h>
#include <vector>

// テスト・サンプル用のインクルードファイル
#include "polygon_sample.hpp"
#include "shader_test.hpp"
#include "common.hpp"
#include "constant_sample.hpp"
#include "pmd_loader.hpp"
#include "material_sample.hpp"
#include "texture_sample.hpp"

// DX12の面倒な処理をまとめている
// しかしDirectX12のバージョンが異なるとコンパイルエラーになることもあるので注意
// すくなくともwindows sdkの19041バージョンなら大丈夫だった
#include "d3dx12.h"

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
static void DebugOutputFormatString(const char* in_p_format, ...)
{
#ifdef _DEBUG
    va_list valist;
    // 変数名をva_listとするとva_list型と混在してコンパイルエラーになるので注意
    va_start(valist, in_p_format);
    vprintf(in_p_format, valist);
    va_end(valist);

    printf("\n");
#endif
}

/// <summary>
/// レイヤーデバッグ機能を有効にする(デバッグ版のみ)
/// </summary>
static void EnableDebugLayer()
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

// DirectX12のインターフェイスファクトリを作成
static IDXGIFactory6* CreateDXGIFactory()
{
    IDXGIFactory6* p_factory = nullptr;
    // 末尾の数字は何？
#ifdef _DEBUG
    auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&p_factory));
    assert(result == S_OK);
#else
    auto result = CreateDXGIFactory1(IID_PPV_ARGS(&p_dxgi_factory));
    assert(result == S_OK);
#endif

    return p_factory;
}

// DirectX12のデバイス作成
ID3D12Device* CreateDXDevice(IDXGIFactory6* in_p_dxgi_factory)
{
    ID3D12Device* p_dev = nullptr;

    // グラフィックスアダプター一覧を取得して利用するアダプターを決める
    // NVIDIAのみしか利用できないようにする
    // 書籍「DirectX12の魔導書」のサンプル動作環境がNVIDIAだから
    std::vector <IDXGIAdapter*> adapters;
    IDXGIAdapter* p_tmp_adapter = nullptr;
    {
        for (int i = 0; in_p_dxgi_factory->EnumAdapters(i, &p_tmp_adapter) != DXGI_ERROR_NOT_FOUND; ++i)
            adapters.push_back(p_tmp_adapter);

        for (auto adpt : adapters)
        {
            DXGI_ADAPTER_DESC adesc = {};
            adpt->GetDesc(&adesc);
            std::wstring strDesc = adesc.Description;
            if (strDesc.find(L"NVIDIA") != std::string::npos) {
                p_tmp_adapter = adpt;
                break;
            }
        }
    }
    assert(p_tmp_adapter);

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
            // 指定したアダプターを利用する
            if (D3D12CreateDevice(p_tmp_adapter, level, IID_PPV_ARGS(&p_dev)) == S_OK)
            {
                feture_level = level;
                break;
            }
        }

        assert(p_dev != nullptr);
    }

    return p_dev;
}

static void ExecuteAndCloseCmd(

    UINT64* out_p_fence_val,
    ID3D12Fence* in_p_fence,
    ID3D12CommandQueue* in_p_cmd_queue,
    ID3D12GraphicsCommandList* in_p_cmd_list)
{
    // これ以上コマンドをためれないようにする
    in_p_cmd_list->Close();
    // コマンドリストの実行
    ID3D12CommandList* p_cmd_lists[] = { in_p_cmd_list };
    in_p_cmd_queue->ExecuteCommandLists(1, p_cmd_lists);

    // コマンド実行のフェンスを通知
    in_p_cmd_queue->Signal(in_p_fence, ++(*out_p_fence_val));
    {
        if (in_p_fence->GetCompletedValue() != (*out_p_fence_val))
        {
            // コマンドリストの命令が全て終了の待ち状態を作る
            auto event = CreateEvent(nullptr, false, false, nullptr);
            in_p_fence->SetEventOnCompletion((*out_p_fence_val), event);

            // イベントが実行されるまで待ち
            WaitForSingleObject(event, INFINITE);

            CloseHandle(event);
        }
    }
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
    // 必ず必要!
    // テクスチャをロードする前に必ず呼び出さないとサポートなしとしてエラーになるので注意！
    auto result_hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    assert(SUCCEEDED(result_hr));

    DebugOutputFormatString("DirectX12 Sample.");

    // 頂点レイアウト定義
    // ※ 頂点レイアウトに合わせて頂点シェーダーが受け取るパラメータも変えないとバグる
    D3D12_INPUT_ELEMENT_DESC input_layout[] = {
        // 座標情報
        {
            // 予約セマンティック名
            "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },

        // 法線情報
        {
            // 予約セマンティック名
            "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },

        // uv情報
        {
            // 予約セマンティック名
            "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },

        // ボーン番号
        {
            // 独自セマンティック名
            "BONE_NO", 0, DXGI_FORMAT_R16G16_UINT, 0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },

        // ボーン影響度
        {
            // 独自セマンティック名
            "WEIGHT", 0, DXGI_FORMAT_R8_UINT, 0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },

        /*
        // 輪郭線フラグ
        {
            // 独自セマンティック名
            "EDGE_FLG", 0, DXGI_FORMAT_R8_UINT, 0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },
        */
    };
    // --------------------------------------------

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
    IDXGIFactory6* p_dxgi_factory = CreateDXGIFactory();
    // DXのデバイス作成
    ID3D12Device* p_dev = CreateDXDevice(p_dxgi_factory);

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

        D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
        // ガンマ補正ありでレンダリングするように設定している
        rtv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        // ディスクリプタの先頭ハンドルを取得
        D3D12_CPU_DESCRIPTOR_HANDLE handle = p_rtv_heaps->GetCPUDescriptorHandleForHeapStart();
        for (UINT i = 0; i < swap_desc.BufferCount; ++i)
        {
            result = p_swapchain->GetBuffer(i, IID_PPV_ARGS(&back_buffers[i]));
            assert(SUCCEEDED(result));

            // レンダーをディスクリプタを指定して作成
            p_dev->CreateRenderTargetView(
                back_buffers[i],
                // バックバッファとフォーマットが違うのでエラー出力されるので注意
                &rtv_desc,
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

    // ルートシグネチャー(ディスクリプタテーブルをまとめたもの)を作る
    ID3D12RootSignature* p_root_sig = nullptr;
    D3D12_ROOT_PARAMETER root_params[2] = {};

    // PMDモデルのマテリアルに張り付けるテクスチャ数
    const UINT c_pmd_model_texture_num = 4;
    {
        ID3DBlob* p_root_sig_blob = nullptr;

        // テクスチャのサンプラ設定
        // テクスチャバッファがなくてもルートシグネチャーを作るためには必要
        D3D12_STATIC_SAMPLER_DESC sampler_descs[2] = {};
        {
            // 基本となる共通サンプラー
            {
                // 0-1の範囲外になった場合は繰り返すようにする
                // 例えば 1.5とになると0.5, -0.5なら0.5と値が0-1の範囲になるように繰り返す
                auto p_sampler_desc = &sampler_descs[0];
                p_sampler_desc->AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                p_sampler_desc->AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                p_sampler_desc->AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

                p_sampler_desc->BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
                // 線形補間(バイリニア)だとピクセルをぼかす
                p_sampler_desc->Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
                // 最近傍補間(ニアレストネイバー法)ピクセルをぼかさないでくっきり表示させる
                p_sampler_desc->Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;

                p_sampler_desc->MaxLOD = D3D12_FLOAT32_MAX;
                p_sampler_desc->MinLOD = 0.0f;
                // ピクセルシェーダーから見える
                // TODO: しかしこれルートパラメータでも同じ設定をしているが、
                // 仮にサンプラは見えないようにしたらどうなるのかな？
                p_sampler_desc->ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
                p_sampler_desc->ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

                // シェーダースロット番号
                p_sampler_desc->ShaderRegister = 0;
            }

            // トゥーン用のサンプラー
            {
                // 0-1の範囲外になった場合は繰り返すようにする
                // 例えば 1.5とになると0.5, -0.5なら0.5と値が0-1の範囲になるように繰り返す
                auto p_sampler_desc = &sampler_descs[1];
                p_sampler_desc->AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
                p_sampler_desc->AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
                p_sampler_desc->AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

                p_sampler_desc->BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
                // 線形補間(バイリニア)だとピクセルをぼかす
                p_sampler_desc->Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
                // 最近傍補間(ニアレストネイバー法)ピクセルをぼかさないでくっきり表示させる
                p_sampler_desc->Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;

                p_sampler_desc->MaxLOD = D3D12_FLOAT32_MAX;
                p_sampler_desc->MinLOD = 0.0f;
                // ピクセルシェーダーから見える
                // TODO: しかしこれルートパラメータでも同じ設定をしているが、
                // 仮にサンプラは見えないようにしたらどうなるのかな？
                p_sampler_desc->ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
                p_sampler_desc->ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

                // シェーダースロット番号
                p_sampler_desc->ShaderRegister = 1;
            }
        }

        // ディスクリプタレンジを作成
        D3D12_DESCRIPTOR_RANGE desc_tbl_ranges[3] = {};
        // 定数のレジスター設定
        {
            // 定数用レンスター0番
            desc_tbl_ranges[0] = CD3DX12_DESCRIPTOR_RANGE(
                D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
                1,
                0
            );

            // マテリアルの定数バッファを定数用レジスター1番に
            desc_tbl_ranges[1] = CD3DX12_DESCRIPTOR_RANGE(
                D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
                1,
                1
            );

            // テクスチャをレジスタ-0番目に
            desc_tbl_ranges[2] = CD3DX12_DESCRIPTOR_RANGE(
                D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
                // 基本 + sph + spa + toon
                c_pmd_model_texture_num,
                0
            );
        }

        // ※ NVIDIAなどのグラボでないとどうもディスクリプタヒープの複数利用できないみたい
        // マザーボードに埋め込みGPUだとディスクリプタヒープは一つしか利用できないと考えたほうがいい
        // なんて面倒な
        // 以下参考サイト
        // https://dixq.net/forum/viewtopic.php?t=21107
        // ルートパラメータを作成
        {
            // [0]のレンジデータを参照する
            CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(root_params[0], 1, &desc_tbl_ranges[0]);
            // [1] / [2]をひとまとめのレンジデータを参照する
            CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(root_params[1], 2, &desc_tbl_ranges[1]);
        }

        // ルートシグネチャー設定を作成
        auto root_sig_desc = CD3DX12_ROOT_SIGNATURE_DESC(
            // ルートパラメータの数
            _countof(root_params),
            // ルートパラメータ
            root_params,
            // サンプラーパラメータの数
            _countof(sampler_descs),
            // サンプラーパラメータ
            sampler_descs,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
        );

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
        if (FAILED(result))
        {
            std::string error_str;
            Common::OutputErrorBlob(&error_str, result, p_error_blob);
            DebugOutputFormatString(error_str.c_str());
        }
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

    // ビューポート作成
    auto viewport = CD3DX12_VIEWPORT(
        0.0f,
        0.0f,
        static_cast<FLOAT>(window_width),
        static_cast<FLOAT>(window_height));

    // シザー矩形作成
    // ビューポートいっぱい表示させる
    D3D12_RECT scissor_rect = {};
    {
        scissor_rect.top = 0;
        scissor_rect.left = 0;
        scissor_rect.right = scissor_rect.left + window_width;
        scissor_rect.bottom = scissor_rect.top + window_height;
    }

    // 深度バッファとビューの作成
    ID3D12Resource* p_depth_buffer = nullptr;
    ID3D12DescriptorHeap* p_dsv_heap = nullptr;
    {
        // バッファ作成
        {
            // リソース生成内容
            auto res_desc = CD3DX12_RESOURCE_DESC::Tex2D(
                DXGI_FORMAT_D32_FLOAT,
                window_width,
                window_height,
                1,
                0,
                1,
                0,
                D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
            );

            // ヒープのプロパティ内容
            auto heap_property = CD3DX12_HEAP_PROPERTIES(
                D3D12_HEAP_TYPE_DEFAULT, 0, 0);

            // バッファをクリアする内容
            D3D12_CLEAR_VALUE depth_clear_value = {};
            {
                depth_clear_value.DepthStencil.Depth = 1.0f;
                depth_clear_value.Format = DXGI_FORMAT_D32_FLOAT;
            }

            // リソースバッファを作成
            auto result = p_dev->CreateCommittedResource(
                &heap_property,
                D3D12_HEAP_FLAG_NONE,
                &res_desc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                // 作成するバッファのクリアする内容
                &depth_clear_value,
                IID_PPV_ARGS(&p_depth_buffer)
            );
            assert(SUCCEEDED(result));
        }

        // ディスクリプタヒープを作る
        {
            D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
            // 作成するビューを指定
            heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            heap_desc.NumDescriptors = 1;

            auto result = p_dev->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&p_dsv_heap));
            assert(SUCCEEDED(result));
        }

        // ディスクリプタビューを作る
        {
            D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
            dsv_desc.Format = DXGI_FORMAT_D32_FLOAT;
            dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            dsv_desc.Flags = D3D12_DSV_FLAG_NONE;

            p_dev->CreateDepthStencilView(
                p_depth_buffer,
                &dsv_desc,
                p_dsv_heap->GetCPUDescriptorHandleForHeapStart()
            );
        }
    }

    // ここからはモデル表示に必要な処理
    // --------------------------------------------
    // PMDファイルから情報をロード
    PMDLoader::PMDDataPack pmd_data_pack;
    std::vector<Material::Material> materials;
    std::vector<Texture::Resource> tex_resources;
    std::vector<Texture::Resource> sph_resources;
    std::vector<Texture::Resource> spa_resources;
    std::vector<Texture::Resource> toon_resources;
    {
        std::vector<PMDLoader::PMDMaterial> pmd_material;
        const char* p_model_path = "Resources/Model/Miku.pmd";
        //const char* p_model_path = "Resources/Model/Ruka.pmd";
        auto error = PMDLoader::LoadFile(&pmd_data_pack, &pmd_material, p_model_path);
        assert(error == 0);

        // マテリアルデータコピー
        {
            // nullptr用のテクスチャを作成
            // これは共通利用する
            Texture::Resource white_share_texture;
            Texture::Resource black_share_texture;
            Texture::Resource gradation_share_texture;
            {
                auto result = Texture::CreateTextureResourceFromColor(&white_share_texture, p_dev, 0xff);
                assert(result);

                result = Texture::CreateTextureResourceFromColor(&black_share_texture, p_dev, 0x00);
                assert(result);

                result = Texture::CreateTextureResourceFromGradation(&gradation_share_texture, p_dev, 0xff);
                assert(result);
            }

            materials.resize(pmd_material.size());
            tex_resources.resize(pmd_material.size());
            sph_resources.resize(pmd_material.size());
            spa_resources.resize(pmd_material.size());
            toon_resources.resize(pmd_material.size());

            for (size_t i = 0; i < materials.size(); ++i)
            {
                materials[i].indices_num = pmd_material[i].indices_num;
                materials[i].basic.diffuse = pmd_material[i].diffuse;
                materials[i].basic.alpha = pmd_material[i].alpha;
                materials[i].basic.speclar = pmd_material[i].specular;
                materials[i].basic.specularity = pmd_material[i].specularity;
                materials[i].basic.ambient = pmd_material[i].ambiend;

                // テクスチャファイルパスからロードするファイルパスに置き換えてテクスチャロード
                // マテリアルの数分用意
                {
                    // リソースの初期化
                    tex_resources[i] = white_share_texture;
                    sph_resources[i] = white_share_texture;
                    spa_resources[i] = black_share_texture;

                    std::vector<std::string> load_file_paths;
                    if (0 < std::strlen(pmd_material[i].tex_file_path))
                    {
                        load_file_paths.clear();

                        auto tex_file_path_str = std::string(pmd_material[i].tex_file_path);
                        DebugOutputFormatString(tex_file_path_str.c_str());
                        // ファイルパス内に*が入っているか存在する数でチェック
                        if (std::count(tex_file_path_str.begin(), tex_file_path_str.end(), '*') > 0)
                        {
                            // *が入っている場合はファイルパスを分割する
                            auto split_pair = Common::SplitFileName(tex_file_path_str);
                            DebugOutputFormatString(split_pair.first.c_str());
                            DebugOutputFormatString(split_pair.second.c_str());

                            // スフィアファイルパスが存在するのはfirst/secondはどちらかをチェックして
                            // スフィアファイルパスが存在しない方をテクスチャファイルパスとする
                            auto first_file_extention = Common::GetFileExtension(split_pair.first);
                            auto second_file_extention = Common::GetFileExtension(split_pair.second);

                            // ファイルパスは2つある
                            load_file_paths.push_back(split_pair.first);
                            load_file_paths.push_back(split_pair.second);
                        }
                        else
                        {
                            load_file_paths.push_back(tex_file_path_str);
                        }

                        for (auto& load_file_path : load_file_paths)
                        {
                            // ロードする相対ファイルパスを作成
                            auto load_tex_file_path_str = Common::GetPathForAddPathToDirectoryEndPath(p_model_path, load_file_path.c_str());

                            // ロードするテクスチャファイルパス
                            DebugOutputFormatString(("load texture filepath : " + load_tex_file_path_str).c_str());
                            {
                                if (Common::GetFileExtension(load_tex_file_path_str) == "sph")
                                {
                                    auto result = Texture::CreateTextureResourceFromLoadTextureFile(&sph_resources[i], p_dev, load_tex_file_path_str);
                                    assert(result);
                                }
                                else if (Common::GetFileExtension(load_tex_file_path_str) == "spa")
                                {
                                    auto result = Texture::CreateTextureResourceFromLoadTextureFile(&spa_resources[i], p_dev, load_tex_file_path_str);
                                    assert(result);
                                }
                                else
                                {
                                    auto result = Texture::CreateTextureResourceFromLoadTextureFile(&tex_resources[i], p_dev, load_tex_file_path_str);
                                    assert(result);
                                }
                            }
                        }
                    }
                }

                // トゥーン番号からトゥーンテクスチャをロード
                {
                    toon_resources[i] = gradation_share_texture;
                    std::string toon_file_path;
                    // トゥーンテクスチャファイルパスを生成
                    {
                        const char* fmt = "Resources/Model/Toon/toon%02d.bmp";

                        UINT32 file_toon_no = pmd_material[i].toon_idx + 1;
                        int sz = std::snprintf(nullptr, 0, fmt, file_toon_no);
                        toon_file_path.resize(sz + 1); // note +1 for null terminator
                        std::snprintf(&toon_file_path[0], toon_file_path.size(), fmt, file_toon_no);
                    }

                    DebugOutputFormatString(("load toon texture file path: " + toon_file_path).c_str());
                    auto result = Texture::CreateTextureResourceFromLoadTextureFile(&toon_resources[i], p_dev, toon_file_path);
                    assert(result);
                }
            }
        }
    }

    // 頂点データ定義
    // 頂点バッファビュー作成
    D3D12_VERTEX_BUFFER_VIEW vb_view = {};
    {
        // 頂点バッファを生成
        ID3D12Resource* p_vert_buff = DirectX::CreateResourceForHeapUpload(p_dev, pmd_data_pack.vertexs.size());
        assert(p_vert_buff != nullptr);

        // 作った頂点バッファに情報をコピーする
        // コピーするデータの先頭と末尾の型と転送する型を指定
        DirectX::CopyBuffer<std::vector<UINT8>::iterator, UINT8>(p_vert_buff, pmd_data_pack.vertexs.begin(), pmd_data_pack.vertexs.end());
        // 頂点バッファ構成を出力
        DirectX::OutputVerticesBufferView(&vb_view, p_vert_buff, pmd_data_pack.vertexs.size(), pmd_data_pack.vertex_size);
    }

    // 頂点のインデックスバッファビューを作成
    D3D12_INDEX_BUFFER_VIEW ib_view = {};
    {
        // インデックスバッファを作成
        ID3D12Resource* p_idx_buff = nullptr;
        UINT index_buffer_data_size =
            static_cast<UINT>(pmd_data_pack.indices.size()) * sizeof(pmd_data_pack.indices[0]);
        {
            p_idx_buff = DirectX::CreateResourceForHeapUpload(p_dev, index_buffer_data_size);

            // コピーするデータの先頭と末尾の型と転送する型を指定
            DirectX::CopyBuffer<std::vector<UINT16>::iterator, UINT16>(
                p_idx_buff, pmd_data_pack.indices.begin(), pmd_data_pack.indices.end());
        }

        ib_view.BufferLocation = p_idx_buff->GetGPUVirtualAddress();
        // インデックスの要素データ型を16byteにしている
        ib_view.Format = DXGI_FORMAT_R16_UINT;
        ib_view.SizeInBytes = index_buffer_data_size;
    }

    // マテリアル用のディスクリプタヒープとビュー作成
    ID3D12DescriptorHeap* p_material_desc_heap = nullptr;
    {
        // マテリアルバッファを作成
        ID3D12Resource* p_material_buff = nullptr;
        // 256byteアライメントに調整
        size_t basic_size = sizeof(materials[0].basic);
        size_t alignment_size = (basic_size + 0xff) & ~0xff;

        // バッファ作成とバッファにデータ書き込み
        {
            // マテリアルバッファを作成
            // マテリアルの数から256byteアライメントしたサイズで作成
            p_material_buff = DirectX::CreateResourceForHeapUpload(p_dev, alignment_size * materials.size());

            // マップでデータを書き込み
            INT8* p_map_material = nullptr;
            auto result = p_material_buff->Map(0, nullptr, (void**)&p_map_material);
            assert(SUCCEEDED(result));

            for (auto& m : materials)
            {
                // 構造体サイズと定数バッファの256byteアライメントが違う
                *((Material::MaterialForHlsl*)p_map_material) = m.basic;
                // ポイントアドレスは定数バッファのアライメントに従って更新している
                // 構造体サイズでのアドレスオフセット移動は出来ない
                p_map_material += alignment_size;
            }

            p_material_buff->Unmap(0, nullptr);
        }

        // ディスクリプタヒープ作成
        {
            // ディスクリプタヒープとビューは密接に関わっているので一つでも設定をミスるとバグる
            // 定数バッファ + (テクスチャバッファ + スフィアテクスチャ + 加算スフィアテクスチャ)
            UINT32 desc_num = materials.size() * (1 + c_pmd_model_texture_num);
            p_material_desc_heap = Material::CreateDescriptorHeap(p_dev, desc_num);
            // 指定したバッファでマテリアル用のビューを作成
            Material::CreateBufferView(
                p_dev,
                p_material_buff,
                p_material_desc_heap,
                // マテリアルの定数バッファ作成のため
                materials,
                // テクスチャのリソースバッファ作成のため
                tex_resources,
                // スフィアテクスチャのリソースバッファ作成のため
                sph_resources,
                // 加算スフィアテクスチャのリソースバッファ作成のため
                spa_resources,
                // トゥーンテクスチャのリソースバッファ作成のため
                toon_resources,
                alignment_size);
        }
    }

    // シェーダーファイルロード
    ID3D10Blob* p_vs_blob = nullptr;
    ID3D10Blob* p_ps_blob = nullptr;
    {
        std::string error;
        // 頂点シェーダーをロード
        if (!DirectX::LoadVSShaderFileSample(
            &p_vs_blob,
            &error,
            L"Resources/BasicVertexShader.hlsl"))
        {
            DebugOutputFormatString(error.c_str());
        }

        // ピクセルシェーダーをロード
        if (!DirectX::LoadPSShaderFileSample(
            &p_ps_blob,
            &error,
            L"Resources/BasicPixelShader.hlsl"))
        {
            DebugOutputFormatString(error.c_str());
        }
    }

    // シェーダー側に渡すデータ構成
    struct SceneShaderData
    {
        DirectX::XMMATRIX world_mat = DirectX::XMMatrixIdentity();
        DirectX::XMMATRIX view_mat = DirectX::XMMatrixIdentity();
        DirectX::XMMATRIX proj_mat = DirectX::XMMatrixIdentity();
        DirectX::XMFLOAT3 eye = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    };

    DirectX::XMMATRIX mat = DirectX::XMMatrixIdentity();
    // 3Dの座標変換
    DirectX::XMMATRIX world_mat = DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX view_mat = DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX proj_mat = DirectX::XMMatrixIdentity();
    // 視点
    DirectX::XMFLOAT3 eye(0.0, 18.0f, -20.0);
    {
        // カメラ行列作成
        {
            DirectX::XMFLOAT3 target(0.0, 18.0f, 0.0);
            DirectX::XMFLOAT3 up(0.0, 1.0f, 0.0);

            view_mat = DirectX::XMMatrixLookAtLH(
                DirectX::XMLoadFloat3(&eye),
                DirectX::XMLoadFloat3(&target),
                DirectX::XMLoadFloat3(&up));
        }

        // 射影行列作成
        {
            proj_mat = DirectX::XMMatrixPerspectiveFovLH(
                DirectX::XM_PIDIV2 * 0.1,
                static_cast<float>(window_width) / static_cast<float>(window_height),
                // カメラが写し始めの値
                1.0f,
                // カメラが写し終わりの値
                100.0f);
        }
    }

    ID3D12Resource* p_mat_constant_buff = nullptr;
    SceneShaderData* p_scene_shader_param = nullptr;
    {
        // 定数バッファを作成
        p_mat_constant_buff = Constant::CreateBuffer(p_dev, sizeof(SceneShaderData));
        // バッファにデータを書き込む
        {
            // バッファと書き込む先との関連付け
            p_mat_constant_buff->Map(0, nullptr, (void**)&p_scene_shader_param);
        }
    }

    // 基本ティスクリプタヒープを作る
    auto p_basic_desc_heap = Common::CreateDescriptorHeap(p_dev, 1);
    assert(p_basic_desc_heap != nullptr);

    // ディスクリプタヒープに各リソースビューを作成
    {
        auto basic_heap_handle = p_basic_desc_heap->GetCPUDescriptorHandleForHeapStart();
        // 行列の定数バッファビューを作る
        {
            auto result = Constant::CreateBufferView(p_dev, p_mat_constant_buff, basic_heap_handle);
            assert(result);
        }
    }

    // ----------------------------------------------------

    // グラフィックスパイプラインステートを構築
    ID3D12PipelineState* p_pipeline_state = nullptr;
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};

        // ルートシグネチャーを設定
        gpipeline.pRootSignature = p_root_sig;

        // シェーダー設定
        // TODO: シェーダーを処理中に切り替えたい場合はどうすればいいのか？
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

        // 深度バッファ設定
        // TODO: 現在半透明描画でも深度値が書き込まれてしまっているので半透明は別にしないといけない
        {
            gpipeline.DepthStencilState.DepthEnable = true;
            gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
            gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
            gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
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
            // レンダリングのフォーマットと合わせないとエラー出力する
            gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        }

        // アンチエイリアシング設定
        {
            gpipeline.SampleDesc.Count = 1;
            gpipeline.SampleDesc.Quality = 0;
        }

        auto result = p_dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&p_pipeline_state));
        assert(SUCCEEDED(result));
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
        D3D12_RESOURCE_BARRIER barrier_desc = CD3DX12_RESOURCE_BARRIER::Transition(
            back_buffers[bb_idx],
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        );

        p_cmd_list->ResourceBarrier(1, &barrier_desc);

        p_cmd_list->SetPipelineState(p_pipeline_state);

        // レンダーターゲットビューを設定コマンド追加
        // 参照するディスクリプタのポインターを利用してレンダーターゲットビューを設定
        auto rtv_h = p_rtv_heaps->GetCPUDescriptorHandleForHeapStart();
        auto dsv_h = p_dsv_heap->GetCPUDescriptorHandleForHeapStart();
        {
            rtv_h.ptr += bb_idx * p_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

            p_cmd_list->OMSetRenderTargets(
                1,
                &rtv_h,
                true,
                // 深度バッファを作成したヒープハンドルを設定
                &dsv_h);
        }

        // 深度バッファをクリア
        // 書き込まれた深度値をクリアしないと深度判定がうまくいかない
        {
            p_cmd_list->ClearDepthStencilView(dsv_h, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
        }

        // 設定したレンダーターゲットをクリア
        {
            float clear_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            p_cmd_list->ClearRenderTargetView(rtv_h, clear_color, 0, nullptr);
        }

        p_cmd_list->RSSetViewports(1, &viewport);
        p_cmd_list->RSSetScissorRects(1, &scissor_rect);

        p_cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // 一度に設定できるインデックスバッファビューは一つのみ
        p_cmd_list->IASetIndexBuffer(&ib_view);
        // 頂点バッファビューを設定
        p_cmd_list->IASetVertexBuffers(0, 1, &vb_view);

        // シェーダーに受け渡すパラメータ
        {
            //            world_mat *= DirectX::XMMatrixRotationY(DirectX::XM_PIDIV4 * 0.01f);

            p_scene_shader_param->world_mat = mat * world_mat;
            p_scene_shader_param->view_mat = view_mat;
            p_scene_shader_param->proj_mat = proj_mat;
            p_scene_shader_param->eye = eye;
        }

        p_cmd_list->SetGraphicsRootSignature(p_root_sig);
        {
            // ディスクリプタヒープを設定
            p_cmd_list->SetDescriptorHeaps(1, &p_basic_desc_heap);
            // ルートパラメータ0とディスクリプタヒープの関連付け
            {
                auto basic_heap_handle = p_basic_desc_heap->GetGPUDescriptorHandleForHeapStart();
                p_cmd_list->SetGraphicsRootDescriptorTable(
                    // ルートパラメータインデックス
                    0,
                    // ヒープアドレス
                    basic_heap_handle);
            }

            // マテリアルのディスクリプタヒープ設定
            p_cmd_list->SetDescriptorHeaps(1, &p_material_desc_heap);
            {
                // 一つのディスクリプタヒープにある複数ビューを参照するアドレスオフセットサイズ
                auto cbvsrv_inc_size =
                    p_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * (c_pmd_model_texture_num + 1);

                auto material_h = p_material_desc_heap->GetGPUDescriptorHandleForHeapStart();
                UINT32 idx_offset = 0;
                for (auto& m : materials)
                {
                    p_cmd_list->SetGraphicsRootDescriptorTable(
                        // ルートパラメータインデックス
                        1,
                        // ヒープアドレス
                        material_h);

                    p_cmd_list->DrawIndexedInstanced(m.indices_num, 1, idx_offset, 0, 0);

                    material_h.ptr += cbvsrv_inc_size;
                    idx_offset += m.indices_num;
                }
            }
        }

        // バックバッファをフロントバッファにする
        {
            // リソース指定をPresentにする
            barrier_desc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            barrier_desc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
            p_cmd_list->ResourceBarrier(1, &barrier_desc);
        }

        // コマンドを閉じてからコマンドを実行を一緒にやっている
        // フェンスをやっていてコマンドが終了するまで待機もしている
        ExecuteAndCloseCmd(&fence_val, p_fence, p_cmd_queue, p_cmd_list);

        // バッファを入れ替え
        p_swapchain->Present(1, 0);

        // コマンドリストのリセット
        p_cmd_allocator->Reset();
        // コマンドのクローズ状態を解除
        p_cmd_list->Reset(p_cmd_allocator, nullptr);
    }

    UnregisterClass(w.lpszClassName, w.hInstance);

    return 0;
}
