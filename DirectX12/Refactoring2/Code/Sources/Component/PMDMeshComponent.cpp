#include "Component/PMDMeshComponent.h"

#include "MMD/VMDAnimation.h"
#include "MMD/PMDLoader.h"

#include "Component/SimpleCameraComponent.h"

namespace Component
{
    const std::string PMDMeshCameraComponent::s_center_bone_name = "センター";

    PMDMeshCameraComponent::PMDMeshCameraComponent(
        Actor::Actor* in_pOwner,
        std::shared_ptr<DirectX12::Context> in_context,
        std::shared_ptr<RenderBaseData> in_p_render_data,
        const std::string& in_r_pmd_shader_vs_filepath,
        const std::string& in_r_pmd_shader_ps_filepath)
        : SkeltalMeshCameraComponent(in_pOwner, in_context)
    {
        assert(in_p_render_data);

        this->_base_data = in_p_render_data;

        this->_CreateMesh(
            this->_base_data->vertexs,
            this->_base_data->vertex_size,
            this->_base_data->indices
        );

        // PMD用のシェーダーエフェクト作成
        {
            this->_shader_effect = std::make_shared<PMD::Material::ShaderEffect>();
            this->_shader_effect->Load(this->_context, in_r_pmd_shader_vs_filepath, in_r_pmd_shader_ps_filepath);
            this->_shader_effect->Apply(
                this->_context,
                this->_base_data->pmd_materials,
                this->_base_data->pmd_textures,
                this->_base_data->bone_matrices);
        }
    }

    void PMDMeshCameraComponent::SetAnimation(std::shared_ptr<Animation::BaseAnimation> in_animation)
    {
        SkeltalMeshCameraComponent::SetAnimation(in_animation);
    }

    /// <summary>
    /// ボーンの親から子への行列を反映(再帰処理をする)
    /// </summary>
    /// <param name="in_p_node"></param>
    /// <param name="in_r_mat"></param>
    void PMDMeshCameraComponent::MulBoneMatrixAndRecursive(
        BoneNode* in_p_node, const DirectX::XMMATRIX& in_r_mat, bool in_flg)
    {
        auto render_base_data = this->GetBaseData();
        render_base_data->bone_matrices[in_p_node->index] *= in_r_mat;

        for (auto& r_node : in_p_node->children)
            this->MulBoneMatrixAndRecursive(r_node, render_base_data->bone_matrices[in_p_node->index], in_flg);
    }

    /// <summary>
    /// レンダリング
    /// </summary>
    /// <param name="in_r_local_mat"></param>
    /// <param name="in_r_world_mat"></param>
    /// <param name="in_r_view_mat"></param>
    /// <param name="in_r_proj_mat"></param>
    /// <param name="in_r_cam_pos"></param>
    void PMDMeshCameraComponent::InsertCmdToCmdPipeline(
        const DirectX::XMMATRIX& in_r_local_mat,
        // 座標変換に必要な情報
        const DirectX::XMMATRIX& in_r_world_mat,
        const DirectX::XMMATRIX& in_r_view_mat,
        const DirectX::XMMATRIX& in_r_proj_mat,
        // カメラ視点
        const DirectX::XMFLOAT3& in_r_cam_pos)
    {
        this->_shader_effect->BeginEffect(this->_context);

        // メッシュの描画に必要なコマンドを挿入
        // メッシュだけでは描画できない
        this->_mesh->InsertCmdToCmdPipeline(this->_context, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        // シェーダーに受け渡すパラメータ
        {
            // シーン情報
            auto scene_param = this->_shader_effect->GetSceneParam();
            scene_param->view_mat = in_r_view_mat;
            scene_param->proj_mat = in_r_proj_mat;
            scene_param->eye = in_r_cam_pos;

            // 座標変換
            auto mapped_matrices = this->_shader_effect->GetMappedMatrices();
            mapped_matrices[0] = in_r_local_mat * in_r_world_mat;
        }

        this->_shader_effect->EndEffect(this->_context);
    }

    void PMDMeshCameraComponent::Render()
    {
        // TODO: 描画のビューとプロジェクションの情報をどう渡すかは考えないと
        auto simple_camera_component = this->_pOwner->GetComponent<SimpleCameraComponent*>();
        if (simple_camera_component == nullptr)
            return;

        auto view_mat = simple_camera_component->GetViewMat();
        auto proj_mat = simple_camera_component->GetProjMat();
        auto eye_vec = simple_camera_component->GetEye();

        // レンダリング
        this->InsertCmdToCmdPipeline(
            this->_local_mat._m,
            this->_world_mat._m,
            view_mat._m,
            proj_mat._m,
            eye_vec);
    }
}
