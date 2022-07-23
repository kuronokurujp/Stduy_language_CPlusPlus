#pragma once

#include <memory>
#include <string>
#include <map>

#include "DirectX12/DirectX12MiniHeader.h"

#include "PMDLoader.h"
#include "PMDMaterial.h"

namespace PMD
{
	namespace Render
	{
		// シェーダー側に渡すデータ構成
		struct SceneShaderData
		{
			DirectX::XMMATRIX world_mat = DirectX::XMMatrixIdentity();
			DirectX::XMMATRIX view_mat = DirectX::XMMatrixIdentity();
			DirectX::XMMATRIX proj_mat = DirectX::XMMatrixIdentity();
			DirectX::XMFLOAT3 eye = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		};

		// レンダラー
		class Renderer
		{
		public:
			friend class Factory;

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

		private:
			std::shared_ptr<DirectX12::Context> _context;

			SceneShaderData* _p_scene_shader_param = nullptr;

			::PMD::Loader::PMDDataPack _pmd_data_pack;
			std::vector<::PMD::Material::Material> _pmd_materials;
			std::vector<::PMD::Material::MaterialTexture> _pmd_textures;

			UINT32 _material_desc_num = 0;
			UINT32 _material_texture_num = 0;

			std::string _root_sig_key;
			std::string _gpipeline_key;
			std::string _vs_buff_key;
			std::string _idx_buff_key;

			std::string _basic_buff_key;
			std::string _scene_desc_heap_share_key;

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
			void Terminate();

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

			/// <summary>
			/// 作成したレンダラーを取得
			/// </summary>
			/// <param name="in_r_key"></param>
			/// <returns></returns>
			std::shared_ptr<Renderer> GetRenderer(const std::string& in_r_key);

		private:
			std::shared_ptr<DirectX12::Context> _context;

			// nullptr用のテクスチャを作成
			// これは共通利用する
			DirectX12::Context::ComPtr<ID3D12Resource> _white_share_texture;
			DirectX12::Context::ComPtr<ID3D12Resource> _black_share_texture;
			DirectX12::Context::ComPtr<ID3D12Resource> _gradation_share_texture;

			// 共通で利用するシェーダーキー名
			std::string _vs_shader_key = "pmd_vs";
			std::string _ps_shader_key = "pmd_ps";
		};
	}
}
