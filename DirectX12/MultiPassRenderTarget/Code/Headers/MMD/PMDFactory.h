#pragma once

#include <memory>
#include <string>
#include <map>

#include "DirectX12/DirectX12MiniHeader.h"
#include "DirectX12/DirectX12Mesh.h"

#include "PMDLoader.h"
#include "PMDMaterial.h"

#include "VMDLoader.h"
#include "MMD/VMDAnimation.h"

#include "Component/PMDMeshComponent.h"

namespace PMD
{
    /// <summary>
    /// ファクトリーパターンでPMD関連のオブジェクトを生成
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
        void Terminate();

        /// <summary>
        /// レンダリングに必要データロード
        /// 自動で解放するのでアンロードはいらない
        /// </summary>
        const bool LoadRenderData(
            std::shared_ptr<Component::PMDMeshCameraComponent::RenderBaseData> out_p_render_data,
            const std::string& in_r_pmd_filepath,
            const std::string& in_r_toontex_filepath);

        /// <summary>
        /// モーション作成
        /// </summary>
        /// <param name="in_r_pmd_filepath"></param>
        std::shared_ptr<Animation::BaseAnimation> CreateAnimation(const std::string& in_r_pmd_filepath);

        /// <summary>
        /// レンダリングデータからPMDメッシュコンポーネントを生成
        /// </summary>
        std::shared_ptr<Component::PMDMeshCameraComponent> CreateMeshComponent(
            Actor::Actor* in_p_actor,
            std::shared_ptr<Component::PMDMeshCameraComponent::RenderBaseData> in_p_render_data,
            const std::string& in_r_pmd_shader_vs_filepath,
            const std::string& in_r_pmd_shader_ps_filepath);

        /// <summary>
        /// 指定キー名からPMDデータを返す
        /// </summary>
        std::shared_ptr<::PMD::Loader::PMDDataPack> GetPMDDataPack(std::string in_key) {
            return std::make_shared<::PMD::Loader::PMDDataPack>(_pmd_data_pack_map[in_key]);
        }

    private:
        std::shared_ptr<DirectX12::Context> _context;

        std::map<std::string, ::PMD::Loader::PMDDataPack> _pmd_data_pack_map;

        // nullptr用のテクスチャを作成
        // これは共通利用する
        DirectX12::Context::ComPtr<ID3D12Resource> _white_share_texture;
        DirectX12::Context::ComPtr<ID3D12Resource> _black_share_texture;
        DirectX12::Context::ComPtr<ID3D12Resource> _gradation_share_texture;
    };
}
