#pragma once

#include <memory>
#include <string>
#include <map>

#include "DirectX12/DirectX12MiniHeader.h"
#include "DirectX12/DirectX12Mesh.h"

#include "PMDLoader.h"
#include "PMDMaterial.h"

#include "VMDLoader.h"

namespace PMD
{
    namespace Render
    {
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

        /// <summary>
        /// モーションのキーフレーム
        /// </summary>
        struct MotionKeyFrame
        {
            UINT32 frame_no = 0;
            DirectX::XMVECTOR quaternion = DirectX::XMVectorZero();
            DirectX::XMFLOAT3 offset;
            DirectX::XMFLOAT2 p1;
            DirectX::XMFLOAT2 p2;

            MotionKeyFrame(
                const UINT32 in_frame_no,
                DirectX::XMVECTOR& in_r_que,
                DirectX::XMFLOAT3& in_offset,
                DirectX::XMFLOAT2 in_p1,
                DirectX::XMFLOAT2 in_p2)
                :
                frame_no(in_frame_no),
                quaternion(in_r_que),
                offset(in_offset),
                p1(in_p1),
                p2(in_p2)
            {}
        };

        /// <summary>
        /// PMDモデルのモーション
        /// </summary>
        class Motion
        {
        public:
            friend class Factory;

            void PlayAnimation();
            void UpdateAnimation(class Renderer* in_p_renderer, std::vector<PMD::Loader::PMDIK>& in_r_ik_datas);

        private:
            /// <summary>
            /// 2次元を2点を使ったベジェ曲線計算
            /// </summary>
            const float _CalcBezierByTwo2DPoint(
                const float in_t, const DirectX::XMFLOAT2& in_r_p1, const DirectX::XMFLOAT2& in_r_p2, const UINT32 in_n);

            /// <summary>
            /// IK解決処理
            /// TODO: 作成中
            /// </summary>
            void _IKSolve(class Renderer* in_p_renderer, std::vector<PMD::Loader::PMDIK>& in_r_ik_datas, const UINT32 in_now_frame);

            /// <summary>
            /// CCD-IKによるボーン方向を解決
            /// TODO: 作成中
            /// </summary>
            /// <param name="in_r_ik"></param>
            void _SolveCCDIK(class Renderer* in_p_renderer, const PMD::Loader::PMDIK& in_r_ik);
            /// <summary>
            /// 余弦定理によるボーン方向を解決
            /// TODO: 作成中
            /// </summary>
            /// <param name="in_r_ik"></param>
            void _SolveCosineIK(class Renderer* in_p_renderer, const PMD::Loader::PMDIK& in_r_ik);
            /// <summary>
            /// LockAt行列によるボーン方向を解決
            /// TODO: 作成中
            /// </summary>
            /// <param name="in_r_ik"></param>
            void _SolveLockIK(class Renderer* in_p_renderer, const PMD::Loader::PMDIK& in_r_ik);

        private:
            std::map<std::string, std::vector<MotionKeyFrame>> _motion_key_frames;
            std::vector<VMD::Loader::VMDIkEnable> _ik_enables;
            DWORD _start_time = 0;
            UINT32 _motion_duration = 0;
        };

        struct RenderBaseData
        {
            // ボーン名をボーンインデックスと対応したテーブル
            std::vector<std::string> bone_name_array;
            // ボーンインデックスからボーンノードと対応したテーブル
            std::vector<BoneNode*> bone_node_address_array;
            // GPUに渡すボーン情報
            std::vector<DirectX::XMMATRIX> bone_matrices;

            std::vector<::PMD::Material::MaterialData> pmd_materials;
            std::vector<::PMD::Material::MaterialTexture> pmd_textures;

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

        // レンダラー
        class Renderer
        {
        public:
            // 共通しているルートボーン名
            static const std::string s_center_bone_name;

            friend class Factory;
            friend class Motion;

            Renderer();

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

        public:
            std::shared_ptr<Material::ShaderEffect> _shader_effect;
        private:
            std::shared_ptr<DirectX12::Context> _context;
            std::shared_ptr<DirectX12::Mesh> _mesh;

            // レンダリングの基本データ
            std::shared_ptr<RenderBaseData> _base_data;
        };

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
                std::shared_ptr<RenderBaseData> out_p_render_data,
                const std::string& in_r_pmd_filepath,
                const std::string& in_r_toontex_filepath);

            /// <summary>
            /// モーション作成
            /// </summary>
            /// <param name="in_r_pmd_filepath"></param>
            std::shared_ptr<Motion> CreateMotion(const std::string& in_r_pmd_filepath);

            /// <summary>
            /// PMDファイルを解析してレンダラー作成
            /// TODO: ボーンや頂点情報を収めるMeshクラス, Meshに張り付けるマテリアルクラスを用意してこのメソッドで一括で作成するようにするいずれ
            /// </summary>
            std::shared_ptr<Renderer> CreateRenderer(
                std::shared_ptr<RenderBaseData> in_p_render_data,
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
}
