#include "MMD/PMDRendererFactory.h"

#include "MMD/PMDLoader.h"

#include "DirectX12/DirectX12Math.h"

#include "Common.h"

#include <unordered_map>
#include <tchar.h>
#include <timeapi.h>
#include <array>

namespace PMD
{
    namespace Render
    {
        const std::string Renderer::s_center_bone_name = "センター";

        namespace Impl
        {
            /// <summary>
            /// 指定したマテリアルデータを適応
            /// </summary>
            /// <param name="out_r_source"></param>
            /// <param name="in_r_file_path"></param>
            /// <param name="in_r_toon_file_path_fmt"></param>
            /// <param name="in_r_pmd_material"></param>
            void _ApplyMaterial(
                RenderBaseData* out_p_source,
                std::shared_ptr<DirectX12::Context> in_dx12_context,
                const std::string& in_r_current_resource_path,
                const std::string& in_r_toon_file_path_fmt,
                std::vector<PMD::Loader::PMDMaterial>& in_r_pmd_material,
                DirectX12::Context::ComPtr<ID3D12Resource> in_white_share_texture,
                DirectX12::Context::ComPtr<ID3D12Resource> in_black_share_texture,
                DirectX12::Context::ComPtr<ID3D12Resource> in_gradation_share_texture)
            {
                out_p_source->pmd_materials.resize(in_r_pmd_material.size());
                out_p_source->pmd_textures.resize(in_r_pmd_material.size());

                for (size_t i = 0; i < out_p_source->pmd_materials.size(); ++i)
                {
                    out_p_source->pmd_materials[i].indices_num = in_r_pmd_material[i].indices_num;
                    out_p_source->pmd_materials[i].basic.diffuse = in_r_pmd_material[i].diffuse;
                    out_p_source->pmd_materials[i].basic.alpha = in_r_pmd_material[i].alpha;
                    out_p_source->pmd_materials[i].basic.speclar = in_r_pmd_material[i].specular;
                    out_p_source->pmd_materials[i].basic.specularity = in_r_pmd_material[i].specularity;
                    out_p_source->pmd_materials[i].basic.ambient = in_r_pmd_material[i].ambiend;

                    // リソースの初期化
                    out_p_source->pmd_textures[i].tex = in_white_share_texture;
                    out_p_source->pmd_textures[i].sph = in_white_share_texture;
                    out_p_source->pmd_textures[i].spa = in_black_share_texture;
                    out_p_source->pmd_textures[i].toon = in_gradation_share_texture;

                    // テクスチャファイルパスからロードするファイルパスに置き換えてテクスチャロード
                    // マテリアルの数分用意
                    {
                        std::vector<std::string> load_file_paths;
                        if (0 < std::strlen(in_r_pmd_material[i].tex_file_path))
                        {
                            load_file_paths.clear();

                            auto tex_file_path_str = std::string(in_r_pmd_material[i].tex_file_path);
                            // ファイルパス内に*が入っているか存在する数でチェック
                            if (std::count(tex_file_path_str.begin(), tex_file_path_str.end(), '*') > 0)
                            {
                                // *が入っている場合はファイルパスを分割する
                                auto split_pair = Common::SplitFileName(tex_file_path_str);

                                // スフィアファイルパスが存在するのはfirst/secondはどちらかをチェックして
                                // スフィアファイルパスが存在しない方をテクスチャファイルパスとする
                                auto first_file_extention = Common::GetFileExtension(split_pair.first);
                                auto second_file_extention = Common::GetFileExtension(split_pair.second);

                                // ファイルパスは2つある
                                load_file_paths.push_back(split_pair.first);
                                load_file_paths.push_back(split_pair.second);
                            }
                            else
                            {
                                load_file_paths.push_back(tex_file_path_str);
                            }

                            for (auto& load_file_path : load_file_paths)
                            {
                                // テキスチャファイルパス作成
                                auto load_tex_file_path_str =
                                    Common::CombineDirPath(in_r_current_resource_path, load_file_path.c_str());

                                // ロードするテクスチャファイルパス
                                {
                                    if (Common::GetFileExtension(load_tex_file_path_str) == "sph")
                                        out_p_source->pmd_textures[i].sph = DirectX12::CreateTextureResourceFromLoadTextureFile(in_dx12_context, load_tex_file_path_str);
                                    else if (Common::GetFileExtension(load_tex_file_path_str) == "spa")
                                        out_p_source->pmd_textures[i].spa = DirectX12::CreateTextureResourceFromLoadTextureFile(in_dx12_context, load_tex_file_path_str);
                                    else
                                        out_p_source->pmd_textures[i].tex = DirectX12::CreateTextureResourceFromLoadTextureFile(in_dx12_context, load_tex_file_path_str);
                                }
                            }
                        }
                    }

                    // トゥーン番号からトゥーンテクスチャをロード
                    {
                        std::string toon_file_path;
                        // トゥーンテクスチャファイルパスを生成
                        {
                            UINT32 file_toon_no = in_r_pmd_material[i].toon_idx + 1;
                            int sz = std::snprintf(nullptr, 0, in_r_toon_file_path_fmt.c_str(), file_toon_no);
                            toon_file_path.resize(sz + 1);
                            std::snprintf(&toon_file_path[0], toon_file_path.size(), in_r_toon_file_path_fmt.c_str(), file_toon_no);
                        }

                        out_p_source->pmd_textures[i].toon = DirectX12::CreateTextureResourceFromLoadTextureFile(in_dx12_context, toon_file_path);
                    }
                }
            }

            /// <summary>
            /// 指定したボーンデータを適応
            /// </summary>
            /// <param name="out_p_renderer"></param>
            /// <param name="in_r_pmd_bone"></param>
            void _ApplyBone(
                RenderBaseData* out_p_source,
                std::vector<PMD::Loader::PMDBone>& in_r_pmd_bone)
            {
                // 親インデックスに対応したボーン名の配列
                std::vector<std::string> bone_names(in_r_pmd_bone.size());

                // ボーンテーブル作成
                {
                    out_p_source->bone_name_array.resize(in_r_pmd_bone.size());
                    out_p_source->bone_node_address_array.resize(in_r_pmd_bone.size());

                    out_p_source->knee_idxs.clear();
                    for (size_t i = 0; i < in_r_pmd_bone.size(); ++i)
                    {
                        auto& r_pmd_node = in_r_pmd_bone[i];

                        // 親インデックスに対応したボーン名を設定
                        bone_names[i] = r_pmd_node.bone_name;

                        // ボーン情報からノード情報を設定
                        auto& r_bone_node = out_p_source->bone_node_table[r_pmd_node.bone_name];
                        r_bone_node.index = i;
                        r_bone_node.start_pos = r_pmd_node.pos;
                        r_bone_node.bone_type = static_cast<BoneType>(r_pmd_node.type);
                        r_bone_node.parent_bone = r_pmd_node.parent_no;
                        r_bone_node.ik_parent_bone = r_pmd_node.ik_bone_no;

                        // ボーンインデックスからボーン名を
                        // ボーンインデックスからボーンデータを
                        // すぐにアクセスできるようにするためのテーブルを用意
                        out_p_source->bone_node_address_array[i] = &r_bone_node;
                        out_p_source->bone_name_array[i] = bone_names[i];

                        // TODO: ひざの番号を収集する
                        {
                            std::string bone_name = r_pmd_node.bone_name;
                            if (bone_name.find("ひざ") != std::string::npos)
                            {
                                out_p_source->knee_idxs.emplace_back(i);
                                LOGD << _T("find ひざ: idx => ") << i;
                            }
                        }
                    }
                }

                // ボーンの親子関係を構築
                {
                    for (auto& r_pmd_node : in_r_pmd_bone)
                    {
                        auto parent_no = r_pmd_node.parent_no;
                        // noが不正値かどうかチェック
                        if (in_r_pmd_bone.size() <= parent_no)
                            continue;

                        auto parent_node_name = bone_names[parent_no];

                        // 親ノードに現在参照しているノードをアタッチ
                        out_p_source->bone_node_table[parent_node_name].children.emplace_back(
                            &out_p_source->bone_node_table[r_pmd_node.bone_name]
                        );
                    }
                }

                // 各ボーンの行列作成
                auto& r_bone_matrices = out_p_source->bone_matrices;
                r_bone_matrices.resize(in_r_pmd_bone.size());

                // すべて単位行列で初期化
                std::fill(r_bone_matrices.begin(), r_bone_matrices.end(), DirectX::XMMatrixIdentity());
            }

            /// <summary>
            /// IKのボーン情報をデバッグ出力
            /// </summary>
            /// <param name="in_r_renderer"></param>
            void _DebugPrintByIKBone(RenderBaseData& in_r_data, std::shared_ptr<::PMD::Loader::PMDDataPack> in_r_pmd_data_pack)
            {
                {
                    // ボーンidxからボーン名を取得する関数変数をラムダ式で作る
                    auto get_name_from_idx = [&](UINT16 idx)->std::string
                    {
                        auto it = std::find_if(
                            in_r_data.bone_node_table.begin(),
                            in_r_data.bone_node_table.end(),
                            [idx](const std::pair<std::string, BoneNode>& obj)
                        {
                            return obj.second.index == idx;
                        }
                        );

                        if (it != in_r_data.bone_node_table.end())
                        {
                            return it->first;
                        }

                        return "";
                    };

                    for (auto& ik : in_r_pmd_data_pack.get()->iks)
                    {
                        LOGD << "IKボーン番号:" << ik.bone_idx << ":" << get_name_from_idx(ik.bone_idx);

                        for (auto& node : ik.node_idxs)
                        {
                            LOGD << "\t ノード番号:" << node << ":" << get_name_from_idx(node);
                        }
                    }
                }
            }
        }

        void Motion::PlayAnimation()
        {
            this->_start_time = ::timeGetTime();
        }

        void Motion::UpdateAnimation(Renderer* in_p_renderer, std::vector<PMD::Loader::PMDIK>& in_r_ik_datas)
        {
            auto elapsed_time = static_cast<float>(::timeGetTime() - this->_start_time);
            UINT32 now_frame_no = static_cast<UINT32>(30.0f * (elapsed_time / 1000.0f));
            /*
            LOGD << "elapsed_time => " << elapsed_time;
            LOGD << "frame_no => " << now_frame_no;
            */
            // アニメをループさせる
            if (this->_motion_duration < now_frame_no)
            {
                this->_start_time = ::timeGetTime();
                now_frame_no = 0;
            }

            auto render_fix_data = in_p_renderer->GetBaseData();
            auto& bone_matrices = render_fix_data->bone_matrices;
            std::fill(bone_matrices.begin(), bone_matrices.end(), DirectX::XMMatrixIdentity());

            for (auto& m : this->_motion_key_frames)
            {
                auto& bone_name = m.first;
                {
                    auto find_it = render_fix_data->bone_node_table.find(bone_name.c_str());
                    if (find_it == render_fix_data->bone_node_table.end())
                        continue;
                }

                auto& node = render_fix_data->bone_node_table[bone_name.c_str()];
                auto& key_frame = m.second;

                // 配列末尾からの後方イテレーションで該当するフレームNOのモーション取得
                auto rit = std::find_if(
                    key_frame.rbegin(), key_frame.rend(),
                    [now_frame_no](const MotionKeyFrame& in_r_motion_key_frame)
                {
                    return in_r_motion_key_frame.frame_no <= now_frame_no;
                }
                );

                if (rit == key_frame.rend())
                    continue;

                // 前方方向のイテレーションを取得
                auto it = rit.base();

                DirectX::XMMATRIX rotaion = DirectX::XMMatrixIdentity();
                DirectX::XMVECTOR offs = DirectX::XMLoadFloat3(&rit->offset);
                if (it != key_frame.end())
                {
                    // モーションの線形補間をする
                    auto t = static_cast<float>(now_frame_no - rit->frame_no) / (static_cast<float>(it->frame_no - rit->frame_no));

                    t = this->_CalcBezierByTwo2DPoint(t, it->p1, it->p2, 12);
                    rotaion = DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionSlerp(rit->quaternion, it->quaternion, t));
                    offs = DirectX::XMVectorLerp(offs, DirectX::XMLoadFloat3(&it->offset), t);
                }
                else
                {
                    rotaion = DirectX::XMMatrixRotationQuaternion(rit->quaternion);
                }

                auto& start_pos = node.start_pos;
                auto mat =
                    // 原点へ移動
                    DirectX::XMMatrixTranslation(-start_pos.x, -start_pos.y, -start_pos.z)
                    // 回転
                    * rotaion
                    // 元の位置に戻す
                    * DirectX::XMMatrixTranslation(start_pos.x, start_pos.y, start_pos.z);

                // ボーンに行列を設定
                bone_matrices[node.index] = mat * DirectX::XMMatrixTranslationFromVector(offs);
            }

            // 設定した行列でボーン全体を更新
            {
                auto node = render_fix_data->bone_node_table[PMD::Render::Renderer::s_center_bone_name.c_str()];
                in_p_renderer->MulBoneMatrixAndRecursive(&node, DirectX::XMMatrixIdentity());
            }

            // TODO: IK関連の処理はここで実行しないとだめ
            this->_IKSolve(in_p_renderer, in_r_ik_datas, now_frame_no);

            // シェーダーに渡す行列更新
            std::copy(bone_matrices.begin(), bone_matrices.end(), in_p_renderer->_shader_effect->GetMappedMatrices() + 1);
        }

        const float Motion::_CalcBezierByTwo2DPoint(
            const float in_t, const DirectX::XMFLOAT2& in_r_p1, const DirectX::XMFLOAT2& in_r_p2, const UINT32 in_n)
        {
            if (in_r_p1.x == in_r_p2.x && in_r_p1.y == in_r_p2.y)
                return in_t;

            float t = in_t;
            const float k0 = 1 + 3 * in_r_p1.x - 3 * in_r_p2.x;
            const float k1 = 3 * in_r_p2.x - 6 * in_r_p1.x;
            const float k2 = 3 * in_r_p1.x;

            constexpr float ep = 0.0005f;

            for (UINT32 i = 0; i < in_n; ++i)
            {
                auto ft = k0 * t * t * t + k1 * t * t + k2 * t - in_t;
                if (ft <= ep && ft >= -ep)
                    break;

                t -= ft * 0.5f;
            }

            auto r = 1.0f - t;
            return t * t * t + 3 * t * t * r * in_r_p2.x + 2 * t * r * r * in_r_p1.x;
        }

        /// <summary>
        /// IK解決処理
        /// </summary>
        void Motion::_IKSolve(Renderer* in_p_renderer, std::vector<PMD::Loader::PMDIK>& in_r_ik_datas, const UINT32 in_now_frame)
        {
            // IK有効フラグをチェックしてIK処理するか決める
            auto it = std::find_if(
                this->_ik_enables.rbegin(),
                this->_ik_enables.rend(),
                [in_now_frame](const VMD::Loader::VMDIkEnable& in_r_ik_enable) {
                return in_r_ik_enable.frame_no <= in_now_frame;
            });

            auto render_fix_data = in_p_renderer->GetBaseData();
            for (auto& ik : in_r_ik_datas)
            {
                if (it != this->_ik_enables.rend())
                {
                    auto ik_enable_it = it->ik_enable_table.find(render_fix_data->bone_name_array[ik.bone_idx]);
                    if (ik_enable_it != it->ik_enable_table.end())
                    {
                        if (!ik_enable_it->second)
                            continue;
                    }
                }

                auto children_nodes_count = ik.node_idxs.size();
                switch (children_nodes_count)
                {
                case 0:
                {
                    assert(0);
                    continue;
                }
                case 1:
                {
                    this->_SolveLockIK(in_p_renderer, ik);
                    break;
                }
                case 2:
                {
                    this->_SolveCosineIK(in_p_renderer, ik);
                    break;
                }
                default:
                    this->_SolveCCDIK(in_p_renderer, ik);
                    break;
                }
            }
        }

        void Motion::_SolveCCDIK(Renderer* in_p_renderer, const PMD::Loader::PMDIK& in_r_ik)
        {
            auto work_render = in_p_renderer;
            auto render_fix_data = work_render->GetBaseData();

            // ターゲット
            auto target_bone_node = render_fix_data->bone_node_address_array[in_r_ik.bone_idx];
            auto target_org_pos = DirectX::XMLoadFloat3(&target_bone_node->start_pos);

            // ターゲット座標をワールドからローカル座標系に変換
            const DirectX::XMMATRIX parent_mat = render_fix_data->bone_matrices[render_fix_data->bone_node_address_array[in_r_ik.bone_idx]->ik_parent_bone];
            DirectX::XMVECTOR det;
            auto inv_parent_mat = DirectX::XMMatrixInverse(&det, parent_mat);
            auto target_next_pos = DirectX::XMVector3Transform(
                target_org_pos, render_fix_data->bone_matrices[in_r_ik.bone_idx] * inv_parent_mat);

            // 末端ノード
            auto end_pos = DirectX::XMLoadFloat3(&render_fix_data->bone_node_address_array[in_r_ik.target_idx]->start_pos);

            // 中間ノード(ルートを含む)
            std::vector<DirectX::XMVECTOR> bone_positions;
            {
                for (auto& cidx : in_r_ik.node_idxs)
                {
                    bone_positions.push_back(DirectX::XMLoadFloat3(&render_fix_data->bone_node_address_array[cidx]->start_pos));
                }
            }

            std::vector<DirectX::XMMATRIX> mats(bone_positions.size());
            std::fill(mats.begin(), mats.end(), DirectX::XMMatrixIdentity());

            auto ik_limit = in_r_ik.limit * DirectX::XM_PI;

            // ikに設定されている試行回数繰り返す
            for (int c = 0; c < in_r_ik.iterations; ++c)
            {
                // ターゲットと末端がほぼ一致したら抜ける
                if (DirectX::XMVector3Length(
                    DirectX::XMVectorSubtract(end_pos, target_next_pos)
                ).m128_f32[0] <= Common::Epsilon)
                {
                    break;
                }

                // ボーンを曲げていく
                int bone_size = static_cast<int>(bone_positions.size());
                for (int bidx = 0; bidx < bone_size; ++bidx)
                {
                    const auto& pos = bone_positions[bidx];
                    // 末端ノードまでのベクトル作成
                    auto vec_to_end = DirectX::XMVectorSubtract(end_pos, pos);
                    // ターゲットノードまでのベクトル作成
                    auto vec_to_target = DirectX::XMVectorSubtract(target_next_pos, pos);

                    // 正規化
                    vec_to_end = DirectX::XMVector3Normalize(vec_to_end);
                    vec_to_target = DirectX::XMVector3Normalize(vec_to_target);

                    // この二つのベクトルが同じかどうかチェックして同じならスキップ
                    if (DirectX::XMVector3Length(
                        DirectX::XMVectorSubtract(vec_to_end, vec_to_target)).m128_f32[0] <= Common::Epsilon)
                    {
                        continue;
                    }

                    // 外積計算および角度計算

                    // 回転軸を作成
                    auto cross = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vec_to_end, vec_to_target));

                    // 2つのベクトルの角度(cos)
                    float angle = DirectX::XMVector3AngleBetweenVectors(vec_to_end, vec_to_target).m128_f32[0];

                    // 回転限界のチェック
                    angle = min(angle, ik_limit);
                    // 軸に沿った回転行列作成
                    DirectX::XMMATRIX rot = DirectX::XMMatrixRotationAxis(cross, angle);

                    auto rev_pos = DirectX::XMVectorScale(pos, -1);
                    // pos中心に回転
                    auto mat = DirectX::XMMatrixTranslationFromVector(rev_pos)
                        * rot
                        * DirectX::XMMatrixTranslationFromVector(pos);

                    // 回転行列を保持
                    mats[bidx] *= mat;

                    // 対象点を全て回転
                    // 現在の点から末端まで回転 and 現在点の回転は不要
                    for (auto idx = bidx - 1; idx >= 0; --idx)
                    {
                        bone_positions[idx] = DirectX::XMVector3Transform(bone_positions[idx], mat);
                    }

                    // 先端ノードを回転
                    end_pos = DirectX::XMVector3Transform(end_pos, mat);

                    // ターゲットと末端がほぼ一致したら抜ける
                    if (DirectX::XMVector3Length(
                        DirectX::XMVectorSubtract(end_pos, target_next_pos)
                    ).m128_f32[0] <= Common::Epsilon)
                    {
                        break;
                    }
                }
            }

            // ワールド座標系の変換行列を更新
            {
                int idx = 0;
                for (auto& cidx : in_r_ik.node_idxs)
                {
                    render_fix_data->bone_matrices[cidx] = mats[idx];
                    ++idx;
                }
            }

            // ボーンの親子関係更新
            {
                auto root_node = render_fix_data->bone_node_address_array[in_r_ik.node_idxs.back()];
                in_p_renderer->MulBoneMatrixAndRecursive(root_node, parent_mat, true);
            }
        }

        void Motion::_SolveCosineIK(Renderer* in_p_renderer, const PMD::Loader::PMDIK& in_r_ik)
        {
            auto work_render = in_p_renderer;
            auto render_base_data = work_render->GetBaseData();

            // IK構成点を保存
            std::vector<DirectX::XMVECTOR> positions;
            // IKのそれぞれのボーン間の距離を保存
            std::array<float, 2> edge_lens;

            // 末端ボーンが近づく目標ボーン
            auto& target_node = render_base_data->bone_node_address_array[in_r_ik.bone_idx];
            auto target_pos = DirectX::XMVector3Transform(
                DirectX::XMLoadFloat3(&target_node->start_pos), render_base_data->bone_matrices[in_r_ik.bone_idx]);

            // IKチェーンが逆順らしいので、逆に並べる
            // 末端ボーン
            {
                auto end_node = render_base_data->bone_node_address_array[in_r_ik.target_idx];
                positions.emplace_back(DirectX::XMLoadFloat3(&end_node->start_pos));
            }

            // 中間 or ルートボーン
            {
                for (auto& chain_bone_idx : in_r_ik.node_idxs)
                {
                    auto bone_node = render_base_data->bone_node_address_array[chain_bone_idx];
                    positions.emplace_back(DirectX::XMLoadFloat3(&bone_node->start_pos));
                }
            }

            // 分かりやすくするために並びを逆にする
            std::reverse(positions.begin(), positions.end());

            // ノード同士の辺の長さを計算
            {
                edge_lens[0] = DirectX::XMVector3Length(DirectX::XMVectorSubtract(positions[1], positions[0])).m128_f32[0];
                edge_lens[1] = DirectX::XMVector3Length(DirectX::XMVectorSubtract(positions[2], positions[1])).m128_f32[0];
            }

            // ボーンの座標変換
            {
                // ルートボーン
                positions[0] = DirectX::XMVector3Transform(positions[0], render_base_data->bone_matrices[in_r_ik.node_idxs[1]]);
                // 先端ボーン
                positions[2] = DirectX::XMVector3Transform(positions[2], render_base_data->bone_matrices[in_r_ik.bone_idx]);
            }

            // ルートから先端ボーンのベクトル
            auto lineer_vec = DirectX::XMVectorSubtract(positions[2], positions[0]);

            float A = DirectX::XMVector3Length(lineer_vec).m128_f32[0];
            float B = edge_lens[0];
            float C = edge_lens[1];

            lineer_vec = DirectX::XMVector3Normalize(lineer_vec);

            // ルートから真ん中への角度計算
            float theta1 = acosf((A * A + B * B - C * C) / (2 * A * B));
            // 真ん中から先端への角度計算
            float theta2 = acosf((B * B + C * C - A * A) / (2 * B * C));

            // 軸を求める
            DirectX::XMVECTOR axis;
            {
                if (std::find(
                    render_base_data->knee_idxs.begin(),
                    render_base_data->knee_idxs.end(),
                    in_r_ik.node_idxs[0]) == render_base_data->knee_idxs.end())
                {
                    auto vm = DirectX::XMVector3Normalize(
                        DirectX::XMVectorSubtract(positions[2], positions[0]));
                    auto vt = DirectX::XMVector3Normalize(
                        DirectX::XMVectorSubtract(target_pos, positions[0]));

                    axis = DirectX::XMVector3Cross(vt, vm);
                }
                else
                {
                    axis = DirectX::XMLoadFloat3(&DirectX12::XFloat3Right);
                }
            }

            // 適用
            {
                auto mat1 = DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorScale(positions[0], -1));
                mat1 *= DirectX::XMMatrixRotationAxis(axis, theta1);
                mat1 *= DirectX::XMMatrixTranslationFromVector(positions[0]);

                auto mat2 = DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorScale(positions[1], -1));
                mat2 *= DirectX::XMMatrixRotationAxis(axis, theta2 - DirectX::XM_PI);
                mat2 *= DirectX::XMMatrixTranslationFromVector(positions[1]);

                render_base_data->bone_matrices[in_r_ik.node_idxs[1]] *= mat1;
                render_base_data->bone_matrices[in_r_ik.node_idxs[0]] = mat2 * render_base_data->bone_matrices[in_r_ik.node_idxs[1]];
                render_base_data->bone_matrices[in_r_ik.target_idx] = render_base_data->bone_matrices[in_r_ik.node_idxs[0]];
            }
        }

        void Motion::_SolveLockIK(Renderer* in_p_renderer, const PMD::Loader::PMDIK& in_r_ik)
        {
            auto render_base_data = in_p_renderer->GetBaseData();

            // 制御するノードが一つしかないので配列要素の直接指定でいい
            auto root_node = render_base_data->bone_node_address_array[in_r_ik.node_idxs[0]];
            auto target_node = render_base_data->bone_node_address_array[in_r_ik.target_idx];

            auto rpos1 = DirectX::XMLoadFloat3(&root_node->start_pos);
            auto tpos1 = DirectX::XMLoadFloat3(&target_node->start_pos);

            // ノード先となるボーンの座標を取得
            auto rpos2 = DirectX::XMVector3Transform(rpos1, render_base_data->bone_matrices[in_r_ik.node_idxs[0]]);
            auto tpos2 = DirectX::XMVector3Transform(tpos1, render_base_data->bone_matrices[in_r_ik.bone_idx]);

            // ノードと目的のボーンとの方向ベクトルを取得
            auto origin_vec = DirectX::XMVectorSubtract(tpos1, rpos1);
            auto target_vec = DirectX::XMVectorSubtract(tpos2, rpos2);

            origin_vec = DirectX::XMVector3Normalize(origin_vec);
            target_vec = DirectX::XMVector3Normalize(target_vec);

            DirectX::XMMATRIX mat = DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorScale(rpos2, -1)) *
                DirectX12::LockAtMatrix(origin_vec, target_vec, DirectX12::XFloat3UP, DirectX12::XFloat3Right) *
                DirectX::XMMatrixTranslationFromVector(rpos2);

            // 方向ベクトルに従ってボーンを回転させる
            render_base_data->bone_matrices[in_r_ik.node_idxs[0]] = mat;
        }

        Renderer::Renderer()
        {
        }

        /// <summary>
        /// ボーンの親から子への行列を反映(再帰処理をする)
        /// </summary>
        /// <param name="in_p_node"></param>
        /// <param name="in_r_mat"></param>
        void Renderer::MulBoneMatrixAndRecursive(
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
        void Renderer::InsertCmdToCmdPipeline(
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

        /// <summary>
        /// 初期化
        /// </summary>
        /// <param name="in_r_context"></param>
        /// <returns></returns>
        const bool Factory::Initialize(std::shared_ptr<DirectX12::Context> in_context)
        {
            this->_context = in_context;

            // nullptr用のテクスチャを作成
            // これは共通利用する
            {
                this->_white_share_texture = DirectX12::CreateTextureResourceFromColor(in_context, 0xff);
                this->_black_share_texture = DirectX12::CreateTextureResourceFromColor(in_context, 0x00);
                this->_gradation_share_texture = DirectX12::CreateTextureResourceFromGradation(in_context, 0xff);
            }

            return true;
        }

        /// <summary>
        /// 終了
        /// </summary>
        void Factory::Terminate()
        {
            this->_white_share_texture.Reset();
            this->_black_share_texture.Reset();
            this->_gradation_share_texture.Reset();
        }

        /// <summary>
        /// モーション作成
        /// </summary>
        /// <param name="in_r_pmd_filepath"></param>
        std::shared_ptr<Motion> Factory::CreateMotion(
            const std::string& in_r_pmd_filepath)
        {
            VMD::Loader::VMDDataPack pack;
            VMD::Loader::LoadFileBySync(&pack, in_r_pmd_filepath.c_str());

            std::shared_ptr<Motion> motion = std::make_shared<Motion>();
            motion->_ik_enables.resize(pack.ik_enables.size());
            std::copy(pack.ik_enables.begin(), pack.ik_enables.end(), motion->_ik_enables.begin());

            for (auto& m : pack.motions)
            {
                auto q = DirectX::XMLoadFloat4(&m.quaternion);
                auto add_item = MotionKeyFrame(
                    m.frame_no,
                    q,
                    m.location,
                    DirectX::XMFLOAT2(static_cast<float>(m.bezier[3]) / 127.0f, static_cast<float>(m.bezier[7] / 127.0f)),
                    DirectX::XMFLOAT2(static_cast<float>(m.bezier[11]) / 127.0f, static_cast<float>(m.bezier[15]) / 127.0f)
                );

                motion->_motion_key_frames[m.bone_name].emplace_back(add_item);

                // モーション期間を取得
                motion->_motion_duration = std::max<UINT32>(motion->_motion_duration, add_item.frame_no);
            }

            // モーションキーフレームリストがフレーム番号で昇順で並べ替える
            for (auto& m : motion->_motion_key_frames)
            {
                std::sort(m.second.begin(), m.second.end(),
                    [](const MotionKeyFrame& in_r_first, const MotionKeyFrame& in_r_second)
                {
                    return in_r_first.frame_no <= in_r_second.frame_no;
                });
            }

            return motion;
        }

        const bool Factory::LoadRenderData(
            std::shared_ptr<RenderBaseData> out_p_render_data,
            const std::string& in_r_pmd_filepath,
            const std::string& in_r_toontex_filepath)
        {
            assert(out_p_render_data);

            // PMDファイルを開く
            ::PMD::Loader::PMDDataPack pmd_data_pack;
            {
                // データロード
                auto catch_data = this->_pmd_data_pack_map.find(in_r_pmd_filepath.c_str());
                if (catch_data == this->_pmd_data_pack_map.end())
                {
                    // PMDファイルロード
                    auto error = PMD::Loader::LoadFileBySync(
                        &pmd_data_pack,
                        in_r_pmd_filepath.c_str());
                    assert(error == 0);

                    this->_pmd_data_pack_map[in_r_pmd_filepath.c_str()] = pmd_data_pack;
                }
            }

            {
                out_p_render_data->vertexs = pmd_data_pack.vertexs;
                out_p_render_data->indices = pmd_data_pack.indices;
                out_p_render_data->vertex_size = pmd_data_pack.vertex_size;
                out_p_render_data->iks = pmd_data_pack.iks;

                std::vector<UINT16> indices;
                size_t vertex_size = 0;

                // マテリアルデータ構築
                Impl::_ApplyMaterial(
                    out_p_render_data.get(),
                    this->_context,
                    in_r_pmd_filepath,
                    in_r_toontex_filepath,
                    pmd_data_pack.material,
                    this->_white_share_texture,
                    this->_black_share_texture,
                    this->_gradation_share_texture);

                // ボーンデータ構築
                Impl::_ApplyBone(out_p_render_data.get(), pmd_data_pack.bone);
#ifdef _DEBUG
                // IKに対応しているボーン情報を出力
                Impl::_DebugPrintByIKBone(*out_p_render_data, std::make_shared<::PMD::Loader::PMDDataPack>(pmd_data_pack));
#endif
            }

            return true;
        }

        /// <summary>
        /// PMDファイルを解析してレンダラー作成
        /// </summary>
        std::shared_ptr<Renderer> Factory::CreateRenderer(
            std::shared_ptr<RenderBaseData> in_p_render_data,
            const std::string& in_r_pmd_shader_vs_filepath,
            const std::string& in_r_pmd_shader_ps_filepath)
        {
            std::shared_ptr<Renderer> _renderer = std::make_shared<Renderer>();
            _renderer->_context = this->_context;

            // レンダリングに必要なデータを設定
            _renderer->_base_data = in_p_render_data;

            // メッシュ作成
            {
                _renderer->_mesh = std::make_shared<DirectX12::Mesh>("pmd_vs_buff" + Common::CreateGUIDString(), "pmd_idx_buff" + Common::CreateGUIDString());
                _renderer->_mesh->Create(this->_context, _renderer->_base_data->vertexs, _renderer->_base_data->vertex_size, _renderer->_base_data->indices);
            }

            // PMD用のシェーダーエフェクト作成
            {
                _renderer->_shader_effect = std::make_shared<Material::ShaderEffect>();
                _renderer->_shader_effect->Load(this->_context, in_r_pmd_shader_vs_filepath, in_r_pmd_shader_ps_filepath);
                _renderer->_shader_effect->Apply(
                    this->_context,
                    _renderer->_base_data->pmd_materials,
                    _renderer->_base_data->pmd_textures,
                    _renderer->_base_data->bone_matrices);
            }

            return _renderer;
        }
    }
}
