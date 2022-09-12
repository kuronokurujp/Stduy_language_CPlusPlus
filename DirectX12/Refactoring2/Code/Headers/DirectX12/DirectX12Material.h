#pragma once

#include "DirectX12Core.h"
#include <memory>
#include <string>
#include <functional>

namespace DirectX12
{
    /// <summary>
    /// TODO: マテリアルの構造はシェーダーに依存するので汎用マテリアルの実装はコスト高と判断してこのクラスの実装は保留にする
    /// 汎用マテリアルのクラス
    /// </summary>
    class Material
    {
    public:
        Material();

        bool MakeContentBufferAndViewDesc(
            std::shared_ptr<Context> in_r_context,
            std::string_view in_buff_key,
            const size_t in_buff_size,
            const size_t in_size_in_bytes,
            std::function<void(INT8*)> in_lambda_write_material_buff
        );

        bool AddTextureBufferViewDesc(std::string in_key, D3D12_SHADER_RESOURCE_VIEW_DESC in_view_desc);

        bool Apply(
            std::shared_ptr<Context> in_r_context,
            std::string_view in_heap_key,
            const size_t in_material_num);

    private:
        std::string _constant_buff_key;
        std::string _heap_key;
        std::map<std::string, D3D12_SHADER_RESOURCE_VIEW_DESC> _tex_buff_view_desc_map;
        D3D12_CONSTANT_BUFFER_VIEW_DESC _mat_desc = {};
    };
}
