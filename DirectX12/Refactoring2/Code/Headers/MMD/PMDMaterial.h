#pragma once

#include "DirectX12/DirectX12Texture.h"

#include <DirectXMath.h>
#include <string>

#include <basetsd.h>

namespace PMD
{
    namespace Material
    {
        // シェーダー側に与えるマテリアルデータ
        struct MaterialForHlsl
        {
            DirectX::XMFLOAT3 diffuse;
            float alpha;

            DirectX::XMFLOAT3 speclar;
            float specularity;

            DirectX::XMFLOAT3 ambient;
        };

        // オプションマテリアルデータ
        struct AdditionalMaterial
        {
            std::string tex_path;
            UINT32 toon_idx;
            bool edge_flag;
        };

        // マテリアルデータ
        struct Material
        {
            UINT32 indices_num;
            MaterialForHlsl basic;
            AdditionalMaterial additional;
        };

        // マテリアルで利用するテクスチャ
        struct MaterialTexture
        {
            DirectX12::Context::ComPtr<ID3D12Resource> tex;
            DirectX12::Context::ComPtr<ID3D12Resource> sph;
            DirectX12::Context::ComPtr<ID3D12Resource> spa;
            DirectX12::Context::ComPtr<ID3D12Resource> toon;
        };
        static const UINT c_pmd_model_texture_num = 4;
    }
}
