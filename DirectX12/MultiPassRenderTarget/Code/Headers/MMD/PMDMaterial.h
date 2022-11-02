#pragma once

#include "DirectX12/DirectX12Texture.h"

#include <DirectXMath.h>
#include <string>

#include <basetsd.h>

namespace PMD
{
    namespace Material
    {
        // シェーダー側に渡すデータ構成
        struct SceneData
        {
            DirectX::XMMATRIX view_mat = DirectX::XMMatrixIdentity();
            DirectX::XMMATRIX proj_mat = DirectX::XMMatrixIdentity();
            DirectX::XMFLOAT3 eye = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        };

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
        struct MaterialData
        {
            // メッシュに割り当てたマテリアルのインデックスの数
            UINT32 indices_num;
            MaterialForHlsl basic;
            AdditionalMaterial additional;
        };

        // マテリアルで利用するテクスチャ
        struct MaterialTexture
        {
            // これ配列にした方が良かったかも
            DirectX12::ComPtr<ID3D12Resource> tex;
            DirectX12::ComPtr<ID3D12Resource> sph;
            DirectX12::ComPtr<ID3D12Resource> spa;
            DirectX12::ComPtr<ID3D12Resource> toon;
        };

        static const UINT c_pmd_model_texture_num = 4;

        class ShaderEffect
        {
        public:
            ShaderEffect();

            void Load(
                std::shared_ptr<DirectX12::Context> in_context,
                const std::string_view& in_r_vs_filepath,
                const std::string_view& in_r_ps_filepath);

            void Apply(
                std::shared_ptr<DirectX12::Context> in_context,
                std::vector<::PMD::Material::MaterialData>& in_r_pmd_materials,
                std::vector<::PMD::Material::MaterialTexture>& _pmd_textures,
                // GPUに渡すボーン情報
                std::vector<DirectX::XMMATRIX>& _bone_matrices);

            void BeginEffect(
                std::shared_ptr<DirectX12::Context> in_context
            );

            void EndEffect(
                std::shared_ptr<DirectX12::Context> in_context
            );

            SceneData* GetSceneParam() { return this->_p_scene_shader_param; }
            DirectX::XMMATRIX* GetMappedMatrices() { return this->_p_mapped_matrices; }

        private:
            std::string _vs_filepath;
            std::string _ps_filepath;

            std::string _root_sig_key;
            std::string _gpipeline_key;

            std::string _scene_desc_heap_share_key;
            std::string _basic_buff_key;

            std::string _transform_buff_key;
            std::string _transform_desc_heap_share_key;

            std::string _material_desc_heap_share_key;
            std::string _material_buff_key;

            SceneData* _p_scene_shader_param = nullptr;
            DirectX::XMMATRIX* _p_mapped_matrices = nullptr;

            UINT32 _material_desc_num = 0;

            std::vector<::PMD::Material::MaterialData> _pmd_materials;
        };
    }
}
