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
            void UpdateAnimation(class Renderer* in_p_renderer, std::shared_ptr<std::vector<PMD::Loader::PMDIK>> in_ik_datas);

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
            void _IKSolve(class Renderer* in_p_renderer, std::shared_ptr<std::vector<PMD::Loader::PMDIK>> in_ik_datas, const UINT32 in_now_frame);

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

        // レンダラー
        class Renderer
        {
        public:
            // 共通しているルートボーン名
            static const std::string s_center_bone_name;

            friend class Factory;
            friend class Motion;

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
            void Rendering(
                const DirectX::XMMATRIX& in_r_local_mat,
                // 座標変換に必要な情報
                const DirectX::XMMATRIX& in_r_world_mat,
                const DirectX::XMMATRIX& in_r_view_mat,
                const DirectX::XMMATRIX& in_r_proj_mat,
                // カメラ視点
                const DirectX::XMFLOAT3& in_r_cam_pos);

        public:
            // TODO: getterを作る手間を省くため即参照できるようにした
            // ボーン名をボーンインデックスと対応したテーブル
            std::vector<std::string> _bone_name_array;
            // ボーンインデックスからボーンノードと対応したテーブル
            std::vector<BoneNode*> _bone_node_address_array;
            // GPUに渡すボーン情報
            std::vector<DirectX::XMMATRIX> _bone_matrices;

            // ひざボーンのidx
            std::vector<uint32_t> _knee_idxs;
        private:
            std::shared_ptr<DirectX12::Context> _context;

            DirectX::XMMATRIX* _p_mapped_matrices = nullptr;

            // ボーンノードテーブル
            std::map<std::string, BoneNode> _bone_node_table;

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
            /// モーション作成
            /// </summary>
            /// <param name="in_r_pmd_filepath"></param>
            std::shared_ptr<Motion> CreateMotion(const std::string& in_r_pmd_filepath);

            /// <summary>
            /// PMDファイルを解析してレンダラー作成
            /// TODO: ボーンや頂点情報を収めるMeshクラス, Meshに張り付けるマテリアルクラスを用意してこのメソッドで一括で作成するようにするいずれ
            /// </summary>
            std::shared_ptr<Renderer> CreateRenderer(
                const std::string& in_r_pmd_filepath,
                const std::string& in_r_pmd_shader_vs_filepath,
                const std::string& in_r_pmd_shader_ps_filepath,
                const std::string& in_r_toon_path_fmt);

            /// <summary>
            /// 指定キー名からPMDデータを返す
            /// </summary>
            std::shared_ptr<::PMD::Loader::PMDDataPack> GetPMDDataPack(std::string in_key) {
                return std::make_shared<::PMD::Loader::PMDDataPack>(_pmd_data_pack_map[in_key]);
            }

        private:
            void _ApplyMaterial(
                // TODO: 後でポインターに直す！
                std::shared_ptr<Renderer> out_r_renderer,
                const std::string& in_r_file_path,
                const std::string& in_r_toon_file_path_fmt,
                std::vector<PMD::Loader::PMDMaterial>& in_r_pmd_material);

            /// <summary>
            /// ボーンデータ構築
            /// </summary>
            /// <param name="out_p_renderer"></param>
            /// <param name="in_r_pmd_bone"></param>
            void _ApplyBone(
                Renderer* out_p_renderer,
                std::vector<PMD::Loader::PMDBone>& in_r_pmd_bone);

#ifdef _DEBUG
            void _DebugPrintByIKBone(std::shared_ptr<Renderer> in_r_renderer, std::shared_ptr<::PMD::Loader::PMDDataPack> in_r_pmd_data_pack);
#endif

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
