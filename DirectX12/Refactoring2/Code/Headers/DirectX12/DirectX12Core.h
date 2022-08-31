#pragma once

// DXをインクルード
// これらのファイル群はどの環境にも入っているわけじゃないよね？
// ue4はエンジンプロジェクトの中にDXのファイル群が含まれていた
// TODO: わざわざDX12のライブラリをインストールせずにプロジェクトをコピーすればビルドできるようにしたい
#include <d3d12.h>
#include <dxgi1_6.h>

#include <windows.h>
#include <wrl.h>

#include <string>
#include <memory>
#include <vector>
#include <map>

#include "d3dx12.h"
#include <DirectXMath.h>

namespace DirectX12
{
    // XMMATRIX型は16byteアライメントになっているので
    // XMMATRIX型をクラス変数として定義してクラスをnewするとアライメント問題でハングする可能性がある
    // それを防ぐために16byteアライメントでnewする。
    // 以下のそれを行う構造体
    // newで生成するクラス・構造体の場合では以下のを利用しないといけない
    struct XMMATRIX
    {
        void* operator new(size_t size);
        DirectX::XMMATRIX _m;
    };

    // DirectX12制御のコンテキスト
    struct Context
    {
        // ComPtrを利用して解放時にReleaseを呼び出すようにしている
        // DirectX12のAPI全般で利用
        template<typename T>
        using ComPtr = Microsoft::WRL::ComPtr<T>;

        // ドライバーやディスプレイなどの直接制御APIに利用
        ComPtr<IDXGIFactory6> dxgi_factory;

        // デバイス DirectX12のAPIに利用
        ComPtr<ID3D12Device> dev;

        ComPtr<ID3D12CommandAllocator> cmd_allocator;
        ComPtr<ID3D12GraphicsCommandList> cmd_list;
        ComPtr<ID3D12CommandQueue> cmd_queue;

        // アダプター一覧
        std::vector <IDXGIAdapter*> adapters;

        // フェンス値
        UINT64 fence_val = 0;
        ComPtr<ID3D12Fence> fence;

        // ロードしたシェーダーのBlobマップ
        // 頂点シェーダー
        std::map<std::string, ComPtr<ID3D10Blob>> p_vs_blob_map;
        // ピクセルシェーダー
        std::map<std::string, ComPtr<ID3D10Blob>> p_ps_blob_map;

        // リソースバッファマップ
        std::map<std::string, ComPtr<ID3D12Resource>> res_buff_map;

        // ディスクリプタヒープマップ
        std::map<std::string, ComPtr<ID3D12DescriptorHeap>> desc_heap_map;

        // ロードしたテクスチャリソースをキャッシュ用
        std::map<std::string, ComPtr<ID3D12Resource>> _tex_res_map;

        // 作成したルートシグネチャをキャッシュ
        std::map<std::string, ComPtr<ID3D12RootSignature>> _root_sig_map;

        // パイプラインステートマップ
        std::map<std::string, ComPtr<ID3D12PipelineState>> _pipeline_state_map;
    };

    /// <summary>
    /// コンテキスト作成
    /// 最初に必ず呼び出す
    /// </summary>
    /// <param name="in_p_context"></param>
    /// <returns></returns>
    extern const HRESULT CreateContext(std::shared_ptr<Context> in_p_context);

    /// <summary>
    /// コンテストの破棄
    /// </summary>
    /// <param name="in_context"></param>
    extern void DeleteContext(std::shared_ptr<Context> in_p_context);

    /// <summary>
    /// システム作成
    /// 指定したアダプターでシステムを作る
    /// </summary>
    /// <param name="in_p_context"></param>
    /// <param name="in_adaptr_idx"></param>
    /// <returns></returns>
    extern const HRESULT CreateSystem(std::shared_ptr<Context> in_p_context, const size_t in_adaptr_idx);

    /// <summary>
    /// システムの破棄
    /// </summary>
    /// <param name="in_context"></param>
    extern void DeleteSystem(std::shared_ptr<Context> in_p_context);

    /// <summary>
    /// アダプター名からアダプターインデックス取得
    /// </summary>
    /// <param name="in_p_context"></param>
    /// <param name="in_adapter_name"></param>
    /// <returns></returns>
    extern const size_t GetAdapterIdx(std::shared_ptr<Context> in_p_context, const std::wstring in_adapter_name);

    /// <summary>
    /// コマンドの実行とコマンドを閉じる処理
    /// </summary>
    extern void ExecuteAndCloseCmd(std::shared_ptr<Context> in_p_context);

    /// <summary>
    /// GPUへ転送する空リソース作成
    /// リソース内容は使う側がビューで作る
    /// </summary>
    /// <param name="in_p_context"></param>
    /// <param name="in_r_key"></param>
    /// <param name="in_memory_size"></param>
    /// <returns></returns>
    extern Context::ComPtr<ID3D12Resource> CreateEmptyResourceByGPUTransition(std::shared_ptr<Context> in_p_context, const std::string& in_r_key, const UINT in_memory_size);
    extern bool ReleaseResourceByGPUTransition(std::shared_ptr<Context> in_p_context, const std::string& in_r_key);

    /// <summary>
    /// ディスクリプタヒープを作成
    /// パラメータ設定が複雑なので使う側でパラメータを用意してもらうしかない
    /// </summary>
    /// <param name="in_p_context"></param>
    /// <param name="in_key"></param>
    /// <param name="in_desc"></param>
    /// <returns></returns>
    extern bool CreateDescripterHeap(std::shared_ptr<Context> in_p_context, const std::string& in_r_key, const D3D12_DESCRIPTOR_HEAP_DESC& in_desc);

    /// <summary>
    /// ディスクリプタヒープをまとめるルートシグネチャ作成
    /// </summary>
    /// <returns></returns>
    extern const HRESULT CreateRootSignature(
        std::shared_ptr<Context> in_p_context,
        const std::string& in_r_key,
        ID3DBlob** out_pp_error_blob, const CD3DX12_ROOT_SIGNATURE_DESC& in_r_desc);

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
    extern const HRESULT CreateGraphicsPipeline(
        std::shared_ptr<Context> in_p_context,
        const std::string& in_r_key,
        const std::string& in_use_root_sig_key,
        const std::string& in_use_vs_shader_key,
        const std::string& in_use_ps_shader_key,
        const D3D12_INPUT_ELEMENT_DESC* in_p_input_layouts,
        const size_t in_input_layout_num);

    /// <summary>
    /// Blobからエラーメッセージを出力
    /// </summary>
    extern const bool OutputErrorMessageForBlob(std::string* out_p_error, const HRESULT in_result, ID3DBlob* const in_p_error_blob);
}
