#pragma once

#include "SkeletalMeshComponent.h"

#include "MMD/PMDMaterial.h"

namespace PMD
{
    namespace Loader
    {
        struct PMDIK;
    }
}

namespace Component
{
    /// <summary>
    /// PMD専用のメッシュコンポーネント
    /// </summary>
    class PMDMeshCameraComponent : public SkeltalMeshCameraComponent
    {
    public:
        // ボーン種別
        enum class BoneType
        {
            // 回転
            Rotation,
            // 回転と移動
            RotAndMove,
            // IK
            PMDIK,
            // 未定義
            Undefine,
            // IK影響ボーン
            IKChild,
            // 回転影響ボーン
            RotationChild,
            // IK接続先
            IKDestination,
            // 見えないボーン
            Invisible,
        };

        // ボーンのノード
        struct BoneNode
        {
            // ボーンインデックス
            UINT32 index = 0;
            // ボーン種別
            BoneType bone_type;

            UINT32 parent_bone;
            // IK親ボーン
            UINT32 ik_parent_bone;
            // ボーン基準点
            DirectX::XMFLOAT3 start_pos;
            // ボーン先端点(未使用)
            //DirectX::XMFLOAT3 end_pos;
            // 子のノード
            std::vector<BoneNode*> children;
        };

        struct RenderBaseData
        {
            // ボーン名をボーンインデックスと対応したテーブル
            std::vector<std::string> bone_name_array;
            // ボーンインデックスからボーンノードと対応したテーブル
            std::vector<BoneNode*> bone_node_address_array;
            // GPUに渡すボーン情報
            std::vector<DirectX::XMMATRIX> bone_matrices;

            std::vector<PMD::Material::MaterialData> pmd_materials;
            std::vector<PMD::Material::MaterialTexture> pmd_textures;

            // ボーンノードテーブル
            std::map<std::string, BoneNode> bone_node_table;

            // ひざボーンのidx
            std::vector<uint32_t> knee_idxs;

            // メッシュ構築に必要なデータ
            std::vector<UINT8> vertexs;
            std::vector<UINT16> indices;
            size_t vertex_size = 0;

            std::vector<PMD::Loader::PMDIK> iks;
        };

        // 共通しているルートボーン名
        static const std::string s_center_bone_name;

        PMDMeshCameraComponent(
            Actor::Actor* in_pOwner,
            std::shared_ptr<DirectX12::Context> in_context,
            std::shared_ptr<RenderBaseData> in_p_render_data,
            const std::string& in_r_pmd_shader_vs_filepath,
            const std::string& in_r_pmd_shader_ps_filepath);

        void SetAnimation(std::shared_ptr<Animation::BaseAnimation> in_animation) override final;

        /// <summary>
        /// ボーンの親から子への行列を掛ける(再帰処理をする)
        /// </summary>
        /// <param name="in_p_node"></param>
        /// <param name="in_r_mat"></param>
        void MulBoneMatrixAndRecursive(
            BoneNode* in_p_node, const DirectX::XMMATRIX& in_r_mat, bool in_flg = false);

        /// <summary>
        /// レンダリング
        /// </summary>
        /// <param name="in_r_local_mat"></param>
        /// <param name="in_r_world_mat"></param>
        /// <param name="in_r_view_mat"></param>
        /// <param name="in_r_proj_mat"></param>
        /// <param name="in_r_cam_pos"></param>
        void InsertCmdToCmdPipeline(
            const DirectX::XMMATRIX& in_r_local_mat,
            // 座標変換に必要な情報
            const DirectX::XMMATRIX& in_r_world_mat,
            const DirectX::XMMATRIX& in_r_view_mat,
            const DirectX::XMMATRIX& in_r_proj_mat,
            // カメラ視点
            const DirectX::XMFLOAT3& in_r_cam_pos);

        const std::shared_ptr<RenderBaseData> GetBaseData() { return this->_base_data; }

        /// <summary>
        /// 描画
        /// </summary>
        void Render() override final;

    public:
        std::shared_ptr<PMD::Material::ShaderEffect> _shader_effect;

    private:
        // レンダリングの基本データ
        std::shared_ptr<RenderBaseData> _base_data;
    };
}
