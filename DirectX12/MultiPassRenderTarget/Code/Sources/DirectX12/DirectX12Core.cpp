#include "DirectX12/DirectX12Core.h"

// DX12の面倒な処理をまとめている
// しかしDirectX12のバージョンが異なるとコンパイルエラーになることもあるので注意
// すくなくともwindows sdkの19041バージョンなら大丈夫だった
#include "DirectX12/d3dx12.h"

#include <assert.h>

// DXのライブラリを含める
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

namespace DirectX12
{
    /// <summary>
    /// DirectX::XMMATRIXを16byteアライメントで確保
    /// </summary>
    /// <param name="size"></param>
    /// <returns></returns>
    void* XMMATRIX::operator new(size_t size)
    {
        return _aligned_malloc(size, 16);
    };

    /// <summary>
    /// レイヤーデバッグ機能を有効
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

    /// <summary>
    /// コンテキスト作成
    /// </summary>
    /// <param name="in_p_context"></param>
    /// <returns></returns>
    const HRESULT CreateContext(std::shared_ptr<Context> in_context)
    {
        assert(in_context != nullptr);
        assert(in_context->dxgi_factory.Get() == nullptr);

        // 必ず必要!
        // テクスチャをロードする前に必ず呼び出さないとサポートなしとしてエラーになるので注意！
        auto result_hr = CoInitializeEx(0, COINIT_MULTITHREADED);
        assert(SUCCEEDED(result_hr));

        // 末尾の数字は何？
#ifdef _DEBUG
        auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(in_context->dxgi_factory.ReleaseAndGetAddressOf()));
        // デバッグレイヤーを起動
        EnableDebugLayer();
#else
        auto result = CreateDXGIFactory1(IID_PPV_ARGS(in_p_context->dxgi_factory.ReleaseAndGetAddressOf()));
#endif
        // グラフィックスアダプター一覧を取得
        {
            in_context->adapters.clear();
            IDXGIAdapter* p_tmp_adapter = nullptr;
            for (int i = 0; in_context->dxgi_factory->EnumAdapters(i, &p_tmp_adapter) != DXGI_ERROR_NOT_FOUND; ++i)
                in_context->adapters.push_back(p_tmp_adapter);
        }

        return result;
    }

    /// <summary>
    /// コンテストの破棄
    /// </summary>
    /// <param name="in_context"></param>
    void DeleteContext(std::shared_ptr<Context> in_context)
    {
        assert(in_context != nullptr);

        // コンテキストの破棄をする
        in_context.reset();
    }

    /// <summary>
    /// システム作成
    /// </summary>
    /// <param name="in_context"></param>
    /// <param name="in_adaptr_idx"></param>
    /// <returns></returns>
    const HRESULT CreateSystem(std::shared_ptr<Context> in_context, const size_t in_adaptr_idx)
    {
        auto p_adapter = in_context->adapters[in_adaptr_idx];

        HRESULT result = ((HRESULT)-1L);

        // 失敗は考えない
        {
            const D3D_FEATURE_LEVEL a_levels[] =
            {
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

            for (auto level : a_levels)
            {
                // 指定アダプターを利用する
                result = D3D12CreateDevice(p_adapter.Get(), level, IID_PPV_ARGS(in_context->dev.ReleaseAndGetAddressOf()));
                if (SUCCEEDED(result))
                    break;
            }
        }

        // DXのコマンドリストを生成
        {
            // GPUに命令するためのコマンドリストのデータ
            // メモリサイズは動的に変わるのだろうか？
            result = in_context->dev->CreateCommandAllocator(
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                IID_PPV_ARGS(in_context->cmd_allocator.ReleaseAndGetAddressOf()));
            assert(SUCCEEDED(result));

            // GPUに命令するためのメソッドインターフェイスを作成
            result = in_context->dev->CreateCommandList(
                0,
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                in_context->cmd_allocator.Get(),
                nullptr,
                IID_PPV_ARGS(in_context->cmd_list.ReleaseAndGetAddressOf()));
            assert(SUCCEEDED(result));
        }

        // DXのコマンドリストを実行するコマンドを生成してコマンドキューを生成
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

            result = in_context->dev->CreateCommandQueue(
                &cmd_queue_desc,
                IID_PPV_ARGS(in_context->cmd_queue.ReleaseAndGetAddressOf()));
            assert(SUCCEEDED(result));
        }

        // フェンスオブジェクトを作成
        in_context->fence_val = 0;
        {
            // フェンスを作成
            auto result = in_context->dev->CreateFence(
                in_context->fence_val, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(in_context->fence.ReleaseAndGetAddressOf()));
            assert(SUCCEEDED(result));
        }

        return result;
    }

    /// <summary>
    /// システムの破棄
    /// </summary>
    /// <param name="in_context"></param>
    void DeleteSystem(std::shared_ptr<Context> in_context)
    {
    }

    const size_t GetAdapterIdx(std::shared_ptr<Context> in_context, const std::wstring in_adapter_name)
    {
        DXGI_ADAPTER_DESC adesc = {};
        for (size_t i = 0; i < in_context->adapters.size(); ++i)
        {
            in_context->adapters[i]->GetDesc(&adesc);
            std::wstring str_desc = adesc.Description;
            if (str_desc.find(in_adapter_name.c_str()) != std::string::npos) {
                return i;
            }
        }

        return -1;
    }

    /// <summary>
    /// コマンドの実行とコマンドを閉じる処理
    /// </summary>
    void ExecuteAndCloseCmd(std::shared_ptr<Context> in_p_context)
    {
        // これ以上コマンドをためれないようにする
        in_p_context->cmd_list->Close();
        // コマンドリストの実行
        ID3D12CommandList* p_cmd_lists[] = { in_p_context->cmd_list.Get() };
        in_p_context->cmd_queue->ExecuteCommandLists(1, p_cmd_lists);

        // コマンド実行のフェンスを通知
        in_p_context->cmd_queue->Signal(in_p_context->fence.Get(), ++in_p_context->fence_val);
        {
            if (in_p_context->fence->GetCompletedValue() != in_p_context->fence_val)
            {
                // コマンドリストの命令が全て終了の待ち状態を作る
                auto event = CreateEvent(nullptr, false, false, nullptr);
                in_p_context->fence->SetEventOnCompletion((in_p_context->fence_val), event);

                // イベントが実行されるまで待ち
                WaitForSingleObject(event, INFINITE);

                CloseHandle(event);
            }
        }
    }

    /// <summary>
    /// GPUへ転送する空リソース作成
    /// リソース内容は使う側がビューで作る
    /// </summary>
    /// <param name="in_p_context"></param>
    /// <param name="in_r_key"></param>
    /// <param name="in_memory_size"></param>
    /// <returns></returns>
    ComPtr<ID3D12Resource> CreateEmptyResourceByGPUTransition(
        std::shared_ptr<Context> in_p_context, const std::string& in_r_key, const UINT in_memory_size)
    {
        auto heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto res_desc = CD3DX12_RESOURCE_DESC::Buffer(in_memory_size);

        // 設定内容に基づいて頂点バッファを作成
        ComPtr<ID3D12Resource> p_buff;
        auto result = in_p_context->dev->CreateCommittedResource(
            &heap_prop,
            D3D12_HEAP_FLAG_NONE,
            &res_desc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(p_buff.ReleaseAndGetAddressOf())
        );
        _ASSERTE(SUCCEEDED(result));

        in_p_context->res_buff_map[in_r_key.c_str()] = p_buff;

        return p_buff;
    }

    /// <summary>
    /// GPUへ転送用のリリース削除
    /// </summary>
    /// <param name="in_p_context"></param>
    /// <param name="in_r_key"></param>
    /// <returns></returns>
    bool ReleaseResourceByGPUTransition(std::shared_ptr<Context> in_p_context, const std::string& in_r_key)
    {
        auto res_buff = in_p_context->res_buff_map.find(in_r_key);
        if (res_buff == in_p_context->res_buff_map.cend())
            return false;

        in_p_context->res_buff_map.erase(res_buff);
        res_buff->second.Reset();

        return true;
    }

    /// <summary>
    /// ルートシグネチャ作成
    /// </summary>
    /// <returns></returns>
    const HRESULT CreateRootSignature(
        std::shared_ptr<Context> in_p_context,
        const std::string& in_r_key,
        ID3DBlob** out_pp_error_blob, const CD3DX12_ROOT_SIGNATURE_DESC& in_r_desc)
    {
        ID3DBlob* p_root_sig_blob = nullptr;
        // ルートシグネチャ作成する
        {
            // 最新バージョンから古いバージョン毎に作成
            // 最初に作成に成功したのを利用
            static const D3D_ROOT_SIGNATURE_VERSION s_versions[] =
            {
                D3D_ROOT_SIGNATURE_VERSION_1_1,
                D3D_ROOT_SIGNATURE_VERSION_1_0,
                D3D_ROOT_SIGNATURE_VERSION_1,
            };

            HRESULT h_result;
            for (auto version : s_versions)
            {
                h_result = D3D12SerializeRootSignature(
                    &in_r_desc,
                    // 1_0としているが環境によっては1_1でないとだめなのもあるので本来なら対応しているバージョンを探して設定する必要がある
                    D3D_ROOT_SIGNATURE_VERSION_1_0,
                    &p_root_sig_blob,
                    // 一旦エラーが出ててもなにもしない
                    out_pp_error_blob
                );
                if (SUCCEEDED(h_result))
                    break;
            }

            // 結果を見て成功しているかチェック
            if (FAILED(h_result))
                return h_result;
        }

        ComPtr<ID3D12RootSignature> root_sig;
        HRESULT h_result = S_OK;
        {
            h_result = in_p_context->dev->CreateRootSignature(
                0,
                p_root_sig_blob->GetBufferPointer(),
                p_root_sig_blob->GetBufferSize(),
                IID_PPV_ARGS(root_sig.ReleaseAndGetAddressOf())
            );

            // シグネチャー用のBLOBは不要になったので解放
            p_root_sig_blob->Release();
        }

        in_p_context->_root_sig_map[in_r_key.c_str()] = root_sig;

        return h_result;
    }

    /// <summary>
    /// あらかじめて作成したシェーダー, ルートシグネチャと頂点レイアウトを指定グラフィックパイプラインを作成
    /// </summary>
    /// <param name="in_p_context"></param>
    /// <param name="in_r_key"></param>
    /// <param name="in_use_root_sig_key"></param>
    /// <param name="in_use_vs_shader_key"></param>
    /// <param name="in_use_ps_shader_key"></param>
    /// <param name="in_p_input_layouts"></param>
    /// <param name="in_input_layout_num"></param>
    /// <returns></returns>
    const HRESULT CreateGraphicsPipeline(
        std::shared_ptr<Context> in_p_context,
        const std::string& in_r_key,
        const std::string& in_use_root_sig_key,
        const std::string& in_use_vs_shader_key,
        const std::string& in_use_ps_shader_key,
        const D3D12_INPUT_ELEMENT_DESC* in_p_input_layouts,
        const UINT in_input_layout_num)
    {
        HRESULT h_result;
        // グラフィックスパイプラインステートを構築
        {
            D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};

            // ルートシグネチャーを設定
            gpipeline.pRootSignature = in_p_context->_root_sig_map[in_use_root_sig_key.c_str()].Get();
            assert(gpipeline.pRootSignature != nullptr);

            // シェーダー設定
            {
                auto vs_blob = in_p_context->p_vs_blob_map[in_use_vs_shader_key.c_str()];
                assert(vs_blob != nullptr);
                gpipeline.VS.pShaderBytecode = vs_blob->GetBufferPointer();
                gpipeline.VS.BytecodeLength = vs_blob->GetBufferSize();

                auto ps_blob = in_p_context->p_ps_blob_map[in_use_ps_shader_key.c_str()];
                assert(ps_blob != nullptr);
                gpipeline.PS.pShaderBytecode = ps_blob->GetBufferPointer();
                gpipeline.PS.BytecodeLength = ps_blob->GetBufferSize();
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
            // TODO: いずれ CD3DX12_BLEND_DESC(D3D12_DEFAULT)に置き換えたほうがいいかも
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
                gpipeline.InputLayout.pInputElementDescs = in_p_input_layouts;
                gpipeline.InputLayout.NumElements = in_input_layout_num;
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

            ComPtr<ID3D12PipelineState> p_pipeline_state;
            h_result = in_p_context->dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(p_pipeline_state.ReleaseAndGetAddressOf()));
            assert(SUCCEEDED(h_result));

            in_p_context->_pipeline_state_map[in_r_key.c_str()] = p_pipeline_state;
        }

        return h_result;
    }

    const HRESULT CreateCustomGraphicsPipeline(
        std::shared_ptr<Context> in_p_context,
        const std::string& in_r_key,
        const D3D12_GRAPHICS_PIPELINE_STATE_DESC in_gpipeline,
        const std::string& in_use_root_sig_key,
        const std::string& in_use_vs_shader_key,
        const std::string& in_use_ps_shader_key,
        const D3D12_INPUT_ELEMENT_DESC* in_p_input_layouts,
        const UINT in_input_layout_num)
    {
        HRESULT h_result;
        // グラフィックスパイプラインステートを構築
        {
            D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = in_gpipeline;

            // ルートシグネチャーを設定
            // MEMO: グラフィックパイプラインはルートシグネチャを設定しないとだめなのか？
            // コマンドリストにルートシグネチャを設定してその後にグラフィックパイプラインを設定しているからコマンドリストの設定のみで対応できそうな？
            gpipeline.pRootSignature = in_p_context->_root_sig_map[in_use_root_sig_key.c_str()].Get();
            assert(gpipeline.pRootSignature != nullptr);

            // シェーダー設定
            {
                auto vs_blob = in_p_context->p_vs_blob_map[in_use_vs_shader_key.c_str()];
                assert(vs_blob != nullptr);
                gpipeline.VS.pShaderBytecode = vs_blob->GetBufferPointer();
                gpipeline.VS.BytecodeLength = vs_blob->GetBufferSize();

                auto ps_blob = in_p_context->p_ps_blob_map[in_use_ps_shader_key.c_str()];
                assert(ps_blob != nullptr);
                gpipeline.PS.pShaderBytecode = ps_blob->GetBufferPointer();
                gpipeline.PS.BytecodeLength = ps_blob->GetBufferSize();
            }

            // 入力レイアウトの設定
            {
                gpipeline.InputLayout.pInputElementDescs = in_p_input_layouts;
                gpipeline.InputLayout.NumElements = in_input_layout_num;
            }

            ComPtr<ID3D12PipelineState> p_pipeline_state;
            h_result = in_p_context->dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(p_pipeline_state.ReleaseAndGetAddressOf()));
            assert(SUCCEEDED(h_result));

            in_p_context->_pipeline_state_map[in_r_key.c_str()] = p_pipeline_state;
        }

        return h_result;
    }

    /// <summary>
    /// ディスクリプタヒープを作成
    /// パラメータ設定が複雑なので使う側でパラメータを用意してもらうしかない
    /// </summary>
    /// <param name="in_p_context"></param>
    /// <param name="in_key"></param>
    /// <param name="in_desc"></param>
    /// <returns></returns>
    bool CreateDescripterHeap(
        std::shared_ptr<Context> in_p_context,
        const std::string& in_r_key,
        const D3D12_DESCRIPTOR_HEAP_DESC& in_desc)
    {
        ComPtr<ID3D12DescriptorHeap> p_tex_desc_heap;
        auto result = in_p_context->dev->CreateDescriptorHeap(&in_desc, IID_PPV_ARGS(p_tex_desc_heap.ReleaseAndGetAddressOf()));
        assert(SUCCEEDED(result));

        in_p_context->desc_heap_map[in_r_key.c_str()] = p_tex_desc_heap;

        return true;
    }

    /// <summary>
    /// Blobからエラーメッセージを出力
    /// </summary>
    const bool OutputErrorMessageForBlob(
        std::string* out_p_error,
        const HRESULT in_result,
        ID3DBlob* const in_p_error_blob)
    {
        if (!FAILED(in_result))
            return false;

        if (in_result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
        {
            if (out_p_error != nullptr)
                out_p_error->append("shader file find not");

            return true;
        }

        if (out_p_error != nullptr && in_p_error_blob != nullptr)
        {
            out_p_error->resize(in_p_error_blob->GetBufferSize());
            ::std::copy_n(
                (char*)in_p_error_blob->GetBufferPointer(),
                in_p_error_blob->GetBufferSize(),
                out_p_error->begin());

            return true;
        }

        return false;
    }
}
