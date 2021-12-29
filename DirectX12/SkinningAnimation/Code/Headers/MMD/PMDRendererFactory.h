#pragma once

#include <memory>
#include <string>
#include <map>

#include "DirectX12/DirectX12MiniHeader.h"

#include "PMDLoader.h"
#include "PMDMaterial.h"

#include "VMDLoader.h"

namespace PMD
{
    namespace Render
    {
        // シェーダー側に渡すデータ構成
        struct SceneShaderData
        {
            DirectX::XMMATRIX view_mat = DirectX::XMMatrixIdentity();
            DirectX::XMMATRIX proj_mat = DirectX::XMMatrixIdentity();
            DirectX::XMFLOAT3 eye = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        };

        // ボーンのノード
        struct BoneNode
        {
            // ボーンインデックス
            UINT32 index = 0;
            // ボーン基準点
            DirectX::XMFLOAT3 start_pos;
            // ボーン先端点
            DirectX::XMFLOAT3 end_pos;
            // 子のノード
            std::vector<BoneNode*> children;
        };

        // レンダラー
        class Renderer
        {
        public:
            // 共通しているルートボーン名
            static const std::string _center_bone_name;

            friend class Factory;

            void RecursiveMatrixMuliply(
                BoneNode* in_p_node, const DirectX::XMMATRIX& in_r_mat);

            /// <summary>
            /// レンダリング
            /// </summary>
            /// <param name="in_r_local_mat"></param>
            /// <param name="in_r_world_mat"></param>
            /// <param name="in_r_view_mat"></param>
            /// <param name="in_r_proj_mat"></param>
            /// <param name="in_r_cam_pos"></param>
            void Rendering(
                const DirectX::XMMATRIX& in_r_local_mat,
                // 座標変換に必要な情報
                const DirectX::XMMATRIX& in_r_world_mat,
                const DirectX::XMMATRIX& in_r_view_mat,
                const DirectX::XMMATRIX& in_r_proj_mat,
                // カメラ視点
                const DirectX::XMFLOAT3& in_r_cam_pos);

        public:
            // TODO: 検証のため公開しているので後で直す
            DirectX::XMMATRIX* _p_mapped_matrices = nullptr;

            // GPUに渡すボーン情報j
            std::vector<DirectX::XMMATRIX> _bone_matrices;
            // ボーンノードテーブル
            std::map<std::string, BoneNode> _bone_node_table;

        private:
            std::shared_ptr<DirectX12::Context> _context;

            SceneShaderData* _p_scene_shader_param = nullptr;

            std::vector<::PMD::Material::Material> _pmd_materials;
            std::vector<::PMD::Material::MaterialTexture> _pmd_textures;

            UINT32 _material_desc_num = 0;
            UINT32 _material_texture_num = 0;

            UINT _vb_size_in_bytes = 0;
            UINT _vb_stride_in_bytes = 0;
            UINT _id_size_in_bytes = 0;

            std::string _root_sig_key;
            std::string _gpipeline_key;
            std::string _vs_buff_key;
            std::string _idx_buff_key;

            std::string _basic_buff_key;
            std::string _scene_desc_heap_share_key;

            std::string _transform_buff_key;
            std::string _transform_desc_heap_share_key;

            std::string _material_buff_key;
            std::string _material_desc_heap_share_key;
        };

        /// <summary>
        /// ファクトリーパターンでレンダリング生成を管理
        /// </summary>
        class Factory
        {
        public:

            /// <summary>
            /// 初期化
            /// </summary>
            /// <param name="in_r_context"></param>
            /// <returns></returns>
            const bool Initialize(std::shared_ptr<DirectX12::Context> in_context);

            /// <summary>
            /// 終了
            /// 初期化実行したらこの終了処理を呼ぶ
            /// </summary>
            void Termiate();

            /// <summary>
            /// モーション作成
            /// </summary>
            /// <param name="in_r_pmd_filepath"></param>
            void CreateMotion(
                const std::string& in_r_pmd_filepath);

            /// <summary>
            /// PMDファイルを解析してレンダラー作成
            /// </summary>
            /// <param name="in_r_key"></param>
            /// <param name="in_r_pmd_filepath"></param>
            /// <param name="in_r_pmd_shader_vs_filepath"></param>
            /// <param name="in_r_pmd_shader_ps_filepath"></param>
            /// <returns></returns>
            std::shared_ptr<Renderer> CreateRenderer(
                const std::string& in_r_pmd_filepath,
                const std::string& in_r_pmd_shader_vs_filepath,
                const std::string& in_r_pmd_shader_ps_filepath,
                const std::string& in_r_toon_path_fmt);

        private:
            void _ApplyRenderMaterialData(
                // TODO: 後でポイントに直す！
                std::shared_ptr<Renderer> out_r_renderer,
                const std::string& in_r_file_path,
                const std::string& in_r_toon_file_path_fmt,
                std::vector<PMD::Loader::PMDMaterial>& in_r_pmd_material);

            /// <summary>
            /// ボーンデータ構築
            /// TODO: 作り中
            /// </summary>
            /// <param name="out_p_renderer"></param>
            /// <param name="in_r_pmd_bone"></param>
            void _ApplyRenderBoneData(
                Renderer* out_p_renderer,
                std::vector<PMD::Loader::PMDBone>& in_r_pmd_bone);

        private:
            std::shared_ptr<DirectX12::Context> _context;

            std::map<std::string, ::PMD::Loader::PMDDataPack> _pmd_data_pack_map;
            std::map<std::string, ::VMD::Loader::VMDDataPack> _vmd_data_pack_map;

            // nullptr用のテクスチャを作成
            // これは共通利用する
            DirectX12::Context::ComPtr<ID3D12Resource> _white_share_texture;
            DirectX12::Context::ComPtr<ID3D12Resource> _black_share_texture;
            DirectX12::Context::ComPtr<ID3D12Resource> _gradation_share_texture;
        };
    }
}
