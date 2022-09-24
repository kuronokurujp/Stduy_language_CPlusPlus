#include "DirectX12\DirectX12Material.h"

#include <string>

namespace DirectX12
{
    Material::Material()
    {
    }

    bool Material::MakeContentBufferAndViewDesc(
        std::shared_ptr<Context> in_r_context,
        std::string_view in_buff_key,
        const size_t in_buff_size,
        const size_t in_size_in_bytes,
        std::function<void(INT8*)> in_lambda_write_material_buff
    )
    {
        this->_constant_buff_key = in_buff_key;

        DirectX12::Context::ComPtr<ID3D12Resource> p_material_buff;
        // マテリアル数分のバッファ作成してデータ書き込み
        {
            // マテリアルバッファを作成
            // マテリアルの数から256byteアライメントしたサイズで作成
            // マテリアルの数 * アライメントサイズでバッファ作成している
            p_material_buff = DirectX12::CreateEmptyResourceByGPUTransition(in_r_context, this->_constant_buff_key, in_buff_size);

            // マテリアルデータを書き込み
            INT8* p_map_material = nullptr;
            auto result = p_material_buff->Map(0, nullptr, (void**)&p_map_material);
            assert(SUCCEEDED(result));

            // 呼び出し側でやってもらうか
            in_lambda_write_material_buff(p_map_material);

            p_material_buff->Unmap(0, nullptr);
        }

        // マテリアル定数バッファビューの設定データ
        {
            this->_mat_desc.BufferLocation = p_material_buff->GetGPUVirtualAddress();
            // 定数バッファの一塊サイズ
            this->_mat_desc.SizeInBytes = in_size_in_bytes;
        }

        return true;
    }

    bool Material::AddTextureBufferViewDesc(std::string in_key, D3D12_SHADER_RESOURCE_VIEW_DESC in_view_desc)
    {
        // 利用するテクスチャバッファビューの情報を設定
        this->_tex_buff_view_desc_map[in_key] = in_view_desc;
        return true;
    }

    bool Material::Apply(
        std::shared_ptr<Context> in_r_context,
        std::string_view in_heap_key,
        const size_t in_material_num
    )
    {
        this->_heap_key = in_heap_key;
        INT32 now_desc_num = this->_tex_buff_view_desc_map.size();
        if (!this->_constant_buff_key.empty())
            ++now_desc_num;

        // マテリアル用のディスクリプタヒープ作成
        // マテリアルの数が分かった段階でないと作れない
        {
            // ディスクリプタヒープとビューは密接に関わっているので一つでも設定をミスるとバグる
            // 定数バッファ + (テクスチャバッファ + スフィアテクスチャ + 加算スフィアテクスチャ)
            UINT32 desc_num = in_material_num * now_desc_num;
            ID3D12DescriptorHeap* p_material_heap = nullptr;

            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            {
                desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                desc.NodeMask = 0;
                desc.NumDescriptors = desc_num;
                desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            }
            auto b_result = DirectX12::CreateDescripterHeap(in_r_context, this->_heap_key, desc);
            assert(b_result);
        }

        return true;
    }
}
