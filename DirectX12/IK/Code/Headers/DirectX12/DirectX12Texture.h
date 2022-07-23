#pragma once

#include "DirectX12Core.h"

// 前方宣言
namespace DirectX
{
    struct TexMetadata;
    class ScratchImage;
}

namespace DirectX12
{
    /// <summary>
    /// テクスチャファイルを同期ロード
    /// </summary>
    /// <param name="out_p_metadata"></param>
    /// <param name="out_p_scratch_img"></param>
    /// <param name="in_p_load_filepath"></param>
    /// <returns></returns>
    extern bool LoadTextureFileBySync(
        DirectX::TexMetadata* out_p_metadata,
        DirectX::ScratchImage* out_p_scratch_img,
        const std::string& in_r_load_filepath);

    /// <summary>
    /// テクスチャーバッファを作成
    /// </summary>
    extern Context::ComPtr<ID3D12Resource> CreateTextureBuffer(
        std::shared_ptr<Context> in_p_context,
        UINT64 in_width,
        UINT in_height,
        // 表示する画像フォーマット
        DXGI_FORMAT in_rgba_format,
        // 奥行きと配列サイズ
        UINT16 in_depth_or_array_size,
        // ミップマップレベル
        UINT16 in_mip_levels,
        // TODO: 分からん！
        D3D12_RESOURCE_DIMENSION in_dimension);

    /// <summary>
    /// テクスチャが参照できるヒープ構成にする
    /// </summary>
    /// <param name="in_p_dev"></param>
    /// <param name="in_p_tex_buff"></param>
    /// <param name="in_p_tex_desc_heap"></param>
    /// <param name="in_rgba_format"></param>
    /// <returns></returns>
    extern bool CreateShaderResourceView(
        std::shared_ptr<Context> in_p_context,
        Context::ComPtr<ID3D12Resource> in_p_tex_buff,
        DXGI_FORMAT in_rgba_format,
        D3D12_CPU_DESCRIPTOR_HANDLE& in_r_desc_handle);

    /// <summary>
    /// 画像を転送するためのアップロード用の中間ヒープリソースを作る
    /// </summary>
    /// <param name="in_p_dev"></param>
    /// <param name="in_width"></param>
    /// <returns></returns>
    extern Context::ComPtr<ID3D12Resource> CreateUploadResoucre(
        std::shared_ptr<Context> in_p_context, UINT64 in_width);

    /// <summary>
    /// テクスチャーコピーするためのリソースを作成
    /// </summary>
    /// <param name="in_p_dev"></param>
    /// <param name="in_metadata"></param>
    /// <returns></returns>
    extern Context::ComPtr<ID3D12Resource> CreateCopyTextureResoucre(
        std::shared_ptr<Context> in_p_context, const DirectX::TexMetadata& in_metadata);

    /// <summary>
    /// 単色テクスチャリソース作成
    /// </summary>
    /// <param name="out_p_texture"></param>
    /// <param name="in_p_dev"></param>
    /// <returns></returns>
    extern Context::ComPtr<ID3D12Resource> CreateTextureResourceFromColor(
        std::shared_ptr<Context> in_p_context, UINT8 in_color);

    /// <summary>
    /// 単色のグラデーションテクスチャリソースを作成
    /// </summary>
    /// <param name="out_p_texture"></param>
    /// <param name="in_p_dev"></param>
    /// <param name="in_color"></param>
    /// <returns></returns>
    extern Context::ComPtr<ID3D12Resource> CreateTextureResourceFromGradation(
        std::shared_ptr<Context> in_p_context, UINT8 in_color);

    /// <summary>
    /// テクスチャファイルパスからファイルロードして作成バッファを返す
    /// </summary>
    /// <param name="in_r_file_path"></param>
    /// <returns></returns>
    extern Context::ComPtr<ID3D12Resource> CreateTextureResourceFromLoadTextureFile(
        std::shared_ptr<Context> in_p_context, const std::string& in_r_file_path);
}
