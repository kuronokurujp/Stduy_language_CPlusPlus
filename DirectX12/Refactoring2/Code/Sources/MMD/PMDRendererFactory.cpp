﻿#include "MMD/PMDRendererFactory.h"

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
        constexpr float epsilon = 0.0005f;

        void Motion::PlayAnimation()
        {
            this->_start_time = ::timeGetTime();
        }

        void Motion::UpdateAnimation(Renderer* in_p_renderer, std::shared_ptr<std::vector<PMD::Loader::PMDIK>> in_ik_datas)
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

            auto& bone_matrices = in_p_renderer->_bone_matrices;
            std::fill(bone_matrices.begin(), bone_matrices.end(), DirectX::XMMatrixIdentity());

            for (auto& m : this->_motion_key_frames)
            {
                auto& bone_name = m.first;
                {
                    auto find_it = in_p_renderer->_bone_node_table.find(bone_name.c_str());
                    if (find_it == in_p_renderer->_bone_node_table.end())
                        continue;
                }

                auto& node = in_p_renderer->_bone_node_table[bone_name.c_str()];
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
                auto node = in_p_renderer->_bone_node_table[PMD::Render::Renderer::s_center_bone_name.c_str()];
                in_p_renderer->MulBoneMatrixAndRecursive(&node, DirectX::XMMatrixIdentity());
            }

            // TODO: IK関連の処理はここで実行しないとだめ
            this->_IKSolve(in_p_renderer, in_ik_datas, now_frame_no);

            // シェーダーに渡す行列更新
            std::copy(bone_matrices.begin(), bone_matrices.end(), in_p_renderer->_p_mapped_matrices + 1);
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
        void Motion::_IKSolve(Renderer* in_p_renderer, std::shared_ptr<std::vector<PMD::Loader::PMDIK>> in_ik_datas, const UINT32 in_now_frame)
        {
            // IK有効フラグをチェックしてIK処理するか決める
            auto it = std::find_if(
                this->_ik_enables.rbegin(),
                this->_ik_enables.rend(),
                [in_now_frame](const VMD::Loader::VMDIkEnable& in_r_ik_enable) {
                return in_r_ik_enable.frame_no <= in_now_frame;
            });

            for (auto& ik : *(in_ik_datas.get()))
            {
                if (it != this->_ik_enables.rend())
                {
                    auto ik_enable_it = it->ik_enable_table.find(in_p_renderer->_bone_name_array[ik.bone_idx]);
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

            // ターゲット
            auto target_bone_node = work_render->_bone_node_address_array[in_r_ik.bone_idx];
            auto target_org_pos = DirectX::XMLoadFloat3(&target_bone_node->start_pos);

            // ターゲット座標をワールドからローカル座標系に変換
            const DirectX::XMMATRIX parent_mat = work_render->_bone_matrices[work_render->_bone_node_address_array[in_r_ik.bone_idx]->ik_parent_bone];
            DirectX::XMVECTOR det;
            auto inv_parent_mat = DirectX::XMMatrixInverse(&det, parent_mat);
            auto target_next_pos = DirectX::XMVector3Transform(
                target_org_pos, work_render->_bone_matrices[in_r_ik.bone_idx] * inv_parent_mat);

            // 末端ノード
            auto end_pos = DirectX::XMLoadFloat3(&work_render->_bone_node_address_array[in_r_ik.target_idx]->start_pos);

            // 中間ノード(ルートを含む)
            std::vector<DirectX::XMVECTOR> bone_positions;
            {
                for (auto& cidx : in_r_ik.node_idxs)
                {
                    bone_positions.push_back(DirectX::XMLoadFloat3(&work_render->_bone_node_address_array[cidx]->start_pos));
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
                ).m128_f32[0] <= epsilon)
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
                        DirectX::XMVectorSubtract(vec_to_end, vec_to_target)).m128_f32[0] <= epsilon)
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
                    ).m128_f32[0] <= epsilon)
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
                    work_render->_bone_matrices[cidx] = mats[idx];
                    ++idx;
                }
            }

            // ボーンの親子関係更新
            {
                auto root_node = work_render->_bone_node_address_array[in_r_ik.node_idxs.back()];
                in_p_renderer->MulBoneMatrixAndRecursive(root_node, parent_mat, true);
            }
        }

        void Motion::_SolveCosineIK(Renderer* in_p_renderer, const PMD::Loader::PMDIK& in_r_ik)
        {
            auto work_render = in_p_renderer;

            // IK構成点を保存
            std::vector<DirectX::XMVECTOR> positions;
            // IKのそれぞれのボーン間の距離を保存
            std::array<float, 2> edge_lens;

            // 末端ボーンが近づく目標ボーン
            auto& target_node = work_render->_bone_node_address_array[in_r_ik.bone_idx];
            auto target_pos = DirectX::XMVector3Transform(
                DirectX::XMLoadFloat3(&target_node->start_pos), work_render->_bone_matrices[in_r_ik.bone_idx]);

            // IKチェーンが逆順らしいので、逆に並べる
            // 末端ボーン
            {
                auto end_node = work_render->_bone_node_address_array[in_r_ik.target_idx];
                positions.emplace_back(DirectX::XMLoadFloat3(&end_node->start_pos));
            }

            // 中間 or ルートボーン
            {
                for (auto& chain_bone_idx : in_r_ik.node_idxs)
                {
                    auto bone_node = work_render->_bone_node_address_array[chain_bone_idx];
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
                positions[0] = DirectX::XMVector3Transform(positions[0], work_render->_bone_matrices[in_r_ik.node_idxs[1]]);
                // 先端ボーン
                positions[2] = DirectX::XMVector3Transform(positions[2], work_render->_bone_matrices[in_r_ik.bone_idx]);
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
                    work_render->_knee_idxs.begin(),
                    work_render->_knee_idxs.end(),
                    in_r_ik.node_idxs[0]) == work_render->_knee_idxs.end())
                {
                    auto vm = DirectX::XMVector3Normalize(
                        DirectX::XMVectorSubtract(positions[2], positions[0]));
                    auto vt = DirectX::XMVector3Normalize(
                        DirectX::XMVectorSubtract(target_pos, positions[0]));

                    axis = DirectX::XMVector3Cross(vt, vm);
                }
                else
                {
                    axis = DirectX::XMLoadFloat3(&DirectX12::XFloat3RightVec);
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

                work_render->_bone_matrices[in_r_ik.node_idxs[1]] *= mat1;
                work_render->_bone_matrices[in_r_ik.node_idxs[0]] = mat2 * work_render->_bone_matrices[in_r_ik.node_idxs[1]];
                work_render->_bone_matrices[in_r_ik.target_idx] = work_render->_bone_matrices[in_r_ik.node_idxs[0]];
            }
        }

        void Motion::_SolveLockIK(Renderer* in_p_renderer, const PMD::Loader::PMDIK& in_r_ik)
        {
            // 制御するノードが一つしかないので配列要素の直接指定でいい
            auto root_node = in_p_renderer->_bone_node_address_array[in_r_ik.node_idxs[0]];
            auto target_node = in_p_renderer->_bone_node_address_array[in_r_ik.target_idx];

            auto rpos1 = DirectX::XMLoadFloat3(&root_node->start_pos);
            auto tpos1 = DirectX::XMLoadFloat3(&target_node->start_pos);

            // ノード先となるボーンの座標を取得
            auto rpos2 = DirectX::XMVector3Transform(rpos1, in_p_renderer->_bone_matrices[in_r_ik.node_idxs[0]]);
            auto tpos2 = DirectX::XMVector3Transform(tpos1, in_p_renderer->_bone_matrices[in_r_ik.bone_idx]);

            // ノードと目的のボーンとの方向ベクトルを取得
            auto origin_vec = DirectX::XMVectorSubtract(tpos1, rpos1);
            auto target_vec = DirectX::XMVectorSubtract(tpos2, rpos2);

            origin_vec = DirectX::XMVector3Normalize(origin_vec);
            target_vec = DirectX::XMVector3Normalize(target_vec);

            DirectX::XMMATRIX mat = DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorScale(rpos2, -1)) *
                DirectX12::LockAtMatrix(origin_vec, target_vec, DirectX12::XFloat3UPVec, DirectX12::XFloat3RightVec) *
                DirectX::XMMatrixTranslationFromVector(rpos2);

            // 方向ベクトルに従ってボーンを回転させる
            in_p_renderer->_bone_matrices[in_r_ik.node_idxs[0]] = mat;
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
            this->_bone_matrices[in_p_node->index] *= in_r_mat;

            for (auto& r_node : in_p_node->children)
                this->MulBoneMatrixAndRecursive(r_node, this->_bone_matrices[in_p_node->index], in_flg);
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
            // PMD用のパイプラインステートを設定
            {
                auto pipeline_state = this->_context->_pipeline_state_map[this->_gpipeline_key.c_str()];
                this->_context->cmd_list->SetPipelineState(pipeline_state.Get());
            }

            // メッシュの描画コマンドを挿入
            this->_mesh->InsertCmdToCmdPipeline(this->_context, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            // シェーダーに受け渡すパラメータ
            {
                // シーン情報
                this->_p_scene_shader_param->view_mat = in_r_view_mat;
                this->_p_scene_shader_param->proj_mat = in_r_proj_mat;
                this->_p_scene_shader_param->eye = in_r_cam_pos;

                // 座標変換
                this->_p_mapped_matrices[0] = in_r_local_mat * in_r_world_mat;
            }

            // 利用するディスクリプタと関連付けたルートシグネチャを利用
            auto root_sig = this->_context->_root_sig_map[this->_root_sig_key.c_str()];
            this->_context->cmd_list->SetGraphicsRootSignature(root_sig.Get());
            {
                // ディスクリプタヒープを設定

                // シーンパラメータを設定
                {
                    auto p_basic_desc_heap = this->_context->desc_heap_map[this->_scene_desc_heap_share_key.c_str()];
                    assert(p_basic_desc_heap != nullptr);

                    // ルートパラメータ0とディスクリプタヒープの関連付け
                    this->_context->cmd_list->SetDescriptorHeaps(1, p_basic_desc_heap.GetAddressOf());
                    {
                        auto basic_heap_handle = p_basic_desc_heap->GetGPUDescriptorHandleForHeapStart();
                        this->_context->cmd_list->SetGraphicsRootDescriptorTable(
                            // ルートパラメータインデックス
                            0,
                            // ヒープアドレス
                            basic_heap_handle);
                    }
                }

                // 座標変換パラメータを設定
                {
                    auto p_desc_heap = this->_context->desc_heap_map[this->_transform_desc_heap_share_key.c_str()];
                    assert(p_desc_heap != nullptr);

                    // ルートパラメータ0とディスクリプタヒープの関連付け
                    this->_context->cmd_list->SetDescriptorHeaps(1, p_desc_heap.GetAddressOf());
                    {
                        auto heap_handle = p_desc_heap->GetGPUDescriptorHandleForHeapStart();
                        this->_context->cmd_list->SetGraphicsRootDescriptorTable(
                            // ルートパラメータインデックス
                            1,
                            // ヒープアドレス
                            heap_handle);
                    }
                }

                // マテリアルのディスクリプタヒープ設定
                {
                    auto p_desc_heap = this->_context->desc_heap_map[this->_material_desc_heap_share_key.c_str()];
                    this->_context->cmd_list->SetDescriptorHeaps(1, p_desc_heap.GetAddressOf());
                    {
                        // 一つのディスクリプタヒープにある複数ビューを参照するアドレスオフセットサイズ
                        auto cbvsrv_inc_size =
                            this->_context->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * this->_material_desc_num;

                        auto material_h = p_desc_heap->GetGPUDescriptorHandleForHeapStart();
                        UINT32 idx_offset = 0;
                        for (auto& m : this->_pmd_materials)
                        {
                            this->_context->cmd_list->SetGraphicsRootDescriptorTable(
                                // ルートパラメータインデックス
                                2,
                                // ヒープアドレス
                                material_h);

                            this->_context->cmd_list->DrawIndexedInstanced(m.indices_num, 1, idx_offset, 0, 0);

                            material_h.ptr += cbvsrv_inc_size;
                            idx_offset += m.indices_num;
                        }
                    }
                }
            }
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

        /// <summary>
        /// PMDファイルを解析してレンダラー作成
        /// </summary>
        std::shared_ptr<Renderer> Factory::CreateRenderer(
            const std::string& in_r_pmd_filepath,
            const std::string& in_r_pmd_shader_vs_filepath,
            const std::string& in_r_pmd_shader_ps_filepath,
            const std::string& in_r_toon_path_fmt)
        {
            LOGD << "start create renderer: " + in_r_pmd_filepath;

            std::shared_ptr<Renderer> _renderer = std::make_shared<Renderer>();
            _renderer->_context = this->_context;

            // 各データのデータ識別子ランダム文字列名を生成(被らないようにする)
            {
                _renderer->_root_sig_key = "pmd_root_sig" + Common::CreateGUIDString();
                _renderer->_gpipeline_key = "pmd_gpipeline" + Common::CreateGUIDString();

                _renderer->_vs_buff_key = "pmd_vs_buff" + Common::CreateGUIDString();
                _renderer->_idx_buff_key = "pmd_idx_buff" + Common::CreateGUIDString();

                _renderer->_scene_desc_heap_share_key = "basic_heap" + Common::CreateGUIDString();
                _renderer->_basic_buff_key = "pmd_scene_data" + Common::CreateGUIDString();

                _renderer->_transform_buff_key = "transform_heap" + Common::CreateGUIDString();
                _renderer->_transform_desc_heap_share_key = "pmd_transform_data" + Common::CreateGUIDString();

                _renderer->_material_desc_heap_share_key = "pmd_material_heap" + Common::CreateGUIDString();
                _renderer->_material_buff_key = "pmd_const_material" + Common::CreateGUIDString();
            }

            // PMDファイルを開く
            ::PMD::Loader::PMDDataPack pmd_data_pack;
            {
                // データロード
                {
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
                    else
                    {
                        pmd_data_pack = catch_data->second;
                    }
                }

                // マテリアルデータ構築
                this->_ApplyMaterial(_renderer, in_r_pmd_filepath, in_r_toon_path_fmt, pmd_data_pack.material);

                // ボーンデータ構築
                this->_ApplyBone(_renderer.get(), pmd_data_pack.bone);
#ifdef _DEBUG
                // IKに対応しているボーン情報を出力
                this->_DebugPrintByIKBone(_renderer, std::make_shared<::PMD::Loader::PMDDataPack>(pmd_data_pack));
#endif
            }

            // ティスクリプタヒープを作る
            // しかしマザボのOnGPUだとディスクリプタヒープは1つしか作れないので注意
            {
                // シーンディスクリプタヒープ作成
                {
                    D3D12_DESCRIPTOR_HEAP_DESC desc_heap_desc = {};
                    desc_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                    desc_heap_desc.NodeMask = 0;
                    desc_heap_desc.NumDescriptors = 1;
                    desc_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

                    auto b_result = DirectX12::CreateDescripterHeap(this->_context, _renderer->_scene_desc_heap_share_key, desc_heap_desc);
                    assert(b_result);
                }

                // 座標変換ディスクリプタヒープ作成
                {
                    D3D12_DESCRIPTOR_HEAP_DESC desc_heap_desc = {};
                    desc_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                    desc_heap_desc.NodeMask = 0;
                    desc_heap_desc.NumDescriptors = 1;
                    desc_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

                    auto b_result = DirectX12::CreateDescripterHeap(this->_context, _renderer->_transform_desc_heap_share_key, desc_heap_desc);
                    assert(b_result);
                }

                // PMDマテリアル用のディスクリプタヒープ作成
                // マテリアルの数が分かった段階でないと作れない
                {
                    // ディスクリプタヒープとビューは密接に関わっているので一つでも設定をミスるとバグる
                    // 定数バッファ + (テクスチャバッファ + スフィアテクスチャ + 加算スフィアテクスチャ)
                    UINT32 desc_num = _renderer->_pmd_materials.size() * _renderer->_material_desc_num;
                    ID3D12DescriptorHeap* p_material_heap = nullptr;

                    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
                    {
                        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                        desc.NodeMask = 0;
                        desc.NumDescriptors = desc_num;
                        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
                    }
                    auto b_result = DirectX12::CreateDescripterHeap(this->_context, _renderer->_material_desc_heap_share_key, desc);
                    assert(b_result);
                }
            }

            // シェーダーロード
            {
                ID3D10Blob* out_p_error_blob = nullptr;
                HRESULT h_result;
                {
                    LOGD << "vs file load => " << in_r_pmd_shader_vs_filepath;

                    auto filepath = Common::ConvertStringToWideString(in_r_pmd_shader_vs_filepath);
                    h_result = DirectX12::LoadShaderVSBySync(this->_context, in_r_pmd_shader_vs_filepath, &out_p_error_blob, filepath.c_str());
                    if (FAILED(h_result))
                    {
                        // エラー表示
                        std::string error;
                        DirectX12::OutputErrorMessageForBlob(&error, h_result, out_p_error_blob);

                        LOGD << error;
                        assert(false);
                    }
                }

                {
                    LOGD << "ps file load => " << in_r_pmd_shader_ps_filepath;

                    auto ps_file_path = Common::ConvertStringToWideString(in_r_pmd_shader_ps_filepath);
                    h_result = DirectX12::LoadShaderPSBySync(this->_context, in_r_pmd_shader_ps_filepath, &out_p_error_blob, ps_file_path.c_str());
                    if (FAILED(h_result))
                    {
                        // エラー表示
                        std::string error;
                        DirectX12::OutputErrorMessageForBlob(&error, h_result, out_p_error_blob);

                        LOGD << error;
                        assert(false);
                    }
                }
            }

            // メッシュ作成
            {
                _renderer->_mesh = std::make_shared<DirectX12::Mesh>("pmd_vs_buff" + Common::CreateGUIDString(), "pmd_idx_buff" + Common::CreateGUIDString());
                _renderer->_mesh->Create(this->_context, pmd_data_pack.vertexs, pmd_data_pack.vertex_size, pmd_data_pack.indices);
            }

            // マテリアルバッファを作成
            {
                DirectX12::Context::ComPtr<ID3D12Resource> p_material_buff;
                // 定数バッファはデータ一塊が256byteになっている。
                // なのでバッファに複数のデータを書き込む場合は256byteアライメントに調整しないといけない
                // 工夫すればメモリの無駄使いを解消できるみたいだが、ちょっと分からない
                size_t basic_size = sizeof(_renderer->_pmd_materials[0].basic);
                size_t alignment_size = (basic_size + 0xff) & ~0xff;

                // バッファ作成とバッファにデータ書き込み
                {
                    // マテリアルバッファを作成
                    // マテリアルの数から256byteアライメントしたサイズで作成
                    // マテリアルの数 * アライメントサイズでバッファ作成している
                    p_material_buff = DirectX12::CreateEmptyResourceByGPUTransition(this->_context, _renderer->_material_buff_key, alignment_size * _renderer->_pmd_materials.size());

                    // マテリアルデータを書き込み
                    INT8* p_map_material = nullptr;
                    auto result = p_material_buff->Map(0, nullptr, (void**)&p_map_material);
                    assert(SUCCEEDED(result));

                    for (auto& m : _renderer->_pmd_materials)
                    {
                        // 構造体サイズと定数バッファの256byteアライメントが違う
                        *((PMD::Material::MaterialForHlsl*)p_map_material) = m.basic;
                        // ポイントアドレスは定数バッファのアライメントに従って更新している
                        // 構造体サイズでのアドレスオフセット移動は出来ない
                        p_map_material += alignment_size;
                    }

                    p_material_buff->Unmap(0, nullptr);
                }

                // 作成したディスクリプタヒープにビュー付け
                {
                    // マテリアル定数バッファビューの設定データ
                    D3D12_CONSTANT_BUFFER_VIEW_DESC mat_desc = {};
                    {
                        mat_desc.BufferLocation = p_material_buff->GetGPUVirtualAddress();
                        // 定数バッファの一塊サイズ
                        mat_desc.SizeInBytes = alignment_size;
                    }

                    // テクスチャバッファビューの設定データ
                    D3D12_SHADER_RESOURCE_VIEW_DESC tex_desc = {};
                    {
                        tex_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
                        tex_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                        tex_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                        tex_desc.Texture2D.MipLevels = 1;
                    }

                    D3D12_SHADER_RESOURCE_VIEW_DESC sp_tex_multi_desc = {};
                    {
                        sp_tex_multi_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
                        sp_tex_multi_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                        sp_tex_multi_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                        sp_tex_multi_desc.Texture2D.MipLevels = 1;
                    }

                    D3D12_SHADER_RESOURCE_VIEW_DESC sp_tex_add_desc = {};
                    // MISS: 変数名を間違えて初期化してしまった
                    // spaの変数を利用しているつもりが、sphの変数を利用しまっていた
                    // 一文字違いの変数だとミスとなるので長い変数名にする
                    {
                        sp_tex_add_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
                        sp_tex_add_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                        sp_tex_add_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                        sp_tex_add_desc.Texture2D.MipLevels = 1;
                    }

                    D3D12_SHADER_RESOURCE_VIEW_DESC toon_tex_add_desc = {};
                    {
                        toon_tex_add_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
                        toon_tex_add_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                        toon_tex_add_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                        toon_tex_add_desc.Texture2D.MipLevels = 1;
                    }

                    // あらかじめて作成したディスクリプタヒープにビュー付け
                    auto heap = this->_context->desc_heap_map[_renderer->_material_desc_heap_share_key.c_str()];
                    assert(heap != nullptr);

                    auto heap_h = heap->GetCPUDescriptorHandleForHeapStart();
                    auto inc_size = this->_context->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

                    // マテリアルデータ数分ビューを作成
                    for (size_t i = 0; i < _renderer->_pmd_materials.size(); ++i)
                    {
                        // マテリアルの定数バッファビューを作成
                        this->_context->dev->CreateConstantBufferView(&mat_desc, heap_h);
                        heap_h.ptr += inc_size;

                        // 設定するマテリアルのバッファアドレスを更新
                        mat_desc.BufferLocation += mat_desc.SizeInBytes;

                        // テクスチャをビューに追加
                        {
                            tex_desc.Format = _renderer->_pmd_textures[i].tex->GetDesc().Format;
                            this->_context->dev->CreateShaderResourceView(
                                _renderer->_pmd_textures[i].tex.Get(),
                                &tex_desc,
                                heap_h
                            );

                            heap_h.ptr += inc_size;
                        }

                        // スフィアテクスチャをビューに追加
                        {
                            sp_tex_multi_desc.Format = _renderer->_pmd_textures[i].sph->GetDesc().Format;
                            this->_context->dev->CreateShaderResourceView(
                                _renderer->_pmd_textures[i].sph.Get(),
                                &sp_tex_multi_desc,
                                heap_h
                            );

                            heap_h.ptr += inc_size;
                        }

                        // 加算スフィアテクスチャをビューに追加
                        {
                            sp_tex_add_desc.Format = _renderer->_pmd_textures[i].spa->GetDesc().Format;
                            this->_context->dev->CreateShaderResourceView(
                                _renderer->_pmd_textures[i].spa.Get(),
                                &sp_tex_add_desc,
                                heap_h
                            );

                            heap_h.ptr += inc_size;
                        }

                        // トゥーンテクスチャをビューに追加
                        {
                            toon_tex_add_desc.Format = _renderer->_pmd_textures[i].toon->GetDesc().Format;
                            this->_context->dev->CreateShaderResourceView(
                                _renderer->_pmd_textures[i].toon.Get(),
                                &toon_tex_add_desc,
                                heap_h
                            );

                            heap_h.ptr += inc_size;
                        }
                    }
                }
            }

            // シーン用の定数バッファ / ディスクリプタヒープビュー作成
            {
                {
                    auto buff = DirectX12::CreateEmptyResourceByGPUTransition(this->_context, _renderer->_basic_buff_key, (sizeof(SceneShaderData) + 0xff) & ~0xff);
                    buff->Map(0, nullptr, (void**)&_renderer->_p_scene_shader_param);
                    // マップ解除はしない
                    // 更新毎に書き込まれるから
                }

                // ディスクリプタヒープビュー作成
                {
                    auto heap = this->_context->desc_heap_map[_renderer->_scene_desc_heap_share_key.c_str()];
                    auto heap_handle = heap->GetCPUDescriptorHandleForHeapStart();
                    // 行列の定数バッファビューを作る
                    {
                        D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc = {};

                        auto buff = this->_context->res_buff_map[_renderer->_basic_buff_key.c_str()];
                        cbv_desc.BufferLocation = buff->GetGPUVirtualAddress();
                        LOGD << _T("basic_heap: buff->GetDesc().Width = ") << buff->GetDesc().Width;
                        cbv_desc.SizeInBytes = static_cast<UINT>(buff->GetDesc().Width);

                        // ディスクリプタヒープのハンドルにビュー作成
                        this->_context->dev->CreateConstantBufferView(&cbv_desc, heap_handle);
                    }
                    // このディスクリプタヒープに対してはリソース一つのみ
                }
            }

            // 座標変換用の定数バッファ / ディスクリプタヒープを作成
            {
                size_t buff_size = sizeof(DirectX::XMMATRIX) * (_renderer->_bone_matrices.size() + 1);
                buff_size = (buff_size + 0xff) & ~0xff;
                {
                    // 座標変換とボーンのデータサイズ
                    auto buff = DirectX12::CreateEmptyResourceByGPUTransition(this->_context, _renderer->_transform_buff_key, buff_size);
                    buff->Map(0, nullptr, (void**)&_renderer->_p_mapped_matrices);
                    // マップ解除はしない
                    // 更新毎に書き込まれるから

                    // 先頭はワールド行列
                    _renderer->_p_mapped_matrices[0] = DirectX::XMMatrixIdentity();
                    // 以降はボーン行列
                    // コピー元と先で要素数が違うとバグるので注意
                    std::copy(_renderer->_bone_matrices.begin(), _renderer->_bone_matrices.end(), _renderer->_p_mapped_matrices + 1);
                }

                // ディスクリプタヒープビュー作成
                {
                    auto heap = this->_context->desc_heap_map[_renderer->_transform_desc_heap_share_key.c_str()];
                    auto heap_handle = heap->GetCPUDescriptorHandleForHeapStart();
                    // 定数バッファビューを作る
                    {
                        D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc = {};

                        auto buff = this->_context->res_buff_map[_renderer->_transform_buff_key.c_str()];
                        cbv_desc.BufferLocation = buff->GetGPUVirtualAddress();
                        cbv_desc.SizeInBytes = buff_size;

                        this->_context->dev->CreateConstantBufferView(&cbv_desc, heap_handle);
                    }
                    // このディスクリプタヒープに対してはリソース一つのみ
                }
            }

            // ディスクリプタヒープやサンプラをまとめたルートシグネチャを作成
            {
                // テクスチャサンプラー作成
                D3D12_STATIC_SAMPLER_DESC sampler_descs[2] = {};
                {
                    // 基本となる共通サンプラー
                    {
                        // 0-1の範囲外になった場合は繰り返すようにする
                        // 例えば 1.5とになると0.5, -0.5なら0.5と値が0-1の範囲になるように繰り返す
                        auto p_sampler_desc = &sampler_descs[0];
                        p_sampler_desc->AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                        p_sampler_desc->AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                        p_sampler_desc->AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

                        p_sampler_desc->BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
                        // 線形補間(バイリニア)だとピクセルをぼかす
                        p_sampler_desc->Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
                        // 最近傍補間(ニアレストネイバー法)ピクセルをぼかさないでくっきり表示させる
                        p_sampler_desc->Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;

                        p_sampler_desc->MaxLOD = D3D12_FLOAT32_MAX;
                        p_sampler_desc->MinLOD = 0.0f;
                        // ピクセルシェーダーから見える
                        // TODO: しかしこれルートパラメータでも同じ設定をしているが、
                        // 仮にサンプラは見えないようにしたらどうなるのかな？
                        p_sampler_desc->ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
                        p_sampler_desc->ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

                        // シェーダースロット番号
                        p_sampler_desc->ShaderRegister = 0;
                    }

                    // トゥーン用のサンプラー
                    {
                        // 0-1の範囲外になった場合は繰り返すようにする
                        // 例えば 1.5とになると0.5, -0.5なら0.5と値が0-1の範囲になるように繰り返す
                        auto p_sampler_desc = &sampler_descs[1];
                        p_sampler_desc->AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
                        p_sampler_desc->AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
                        p_sampler_desc->AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

                        p_sampler_desc->BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
                        // 線形補間(バイリニア)だとピクセルをぼかす
                        p_sampler_desc->Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
                        // 最近傍補間(ニアレストネイバー法)ピクセルをぼかさないでくっきり表示させる
                        p_sampler_desc->Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;

                        p_sampler_desc->MaxLOD = D3D12_FLOAT32_MAX;
                        p_sampler_desc->MinLOD = 0.0f;
                        // ピクセルシェーダーから見える
                        // TODO: しかしこれルートパラメータでも同じ設定をしているが、
                        // 仮にサンプラは見えないようにしたらどうなるのかな？
                        p_sampler_desc->ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
                        p_sampler_desc->ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

                        // シェーダースロット番号
                        p_sampler_desc->ShaderRegister = 1;
                    }
                }

                // ルートシグネチャ作成
                DirectX12::Context::ComPtr<ID3D12RootSignature> p_root_sig = nullptr;
                {
                    // ルートシグネチャー(ディスクリプタテーブルをまとめたもの)を作る
                    D3D12_ROOT_PARAMETER root_params[3] = {};

                    // ディスクリプタレンジを作成
                    D3D12_DESCRIPTOR_RANGE desc_tbl_ranges[4] = {};
                    // 定数のレジスター設定
                    {
                        // ディスクリプタヒープ1つ目
                        // 定数用レンスター0番
                        desc_tbl_ranges[0] = CD3DX12_DESCRIPTOR_RANGE(
                            D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
                            1,
                            0
                        );

                        // 定数用レジスター1番
                        // 座標変換
                        desc_tbl_ranges[1] = CD3DX12_DESCRIPTOR_RANGE(
                            D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
                            1,
                            1
                        );

                        // ディスクリプタヒープ2つ目
                        // マテリアルの定数バッファを定数用レジスター2番に
                        desc_tbl_ranges[2] = CD3DX12_DESCRIPTOR_RANGE(
                            D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
                            1,
                            2
                        );

                        // テクスチャをレジスタ-0番目に
                        desc_tbl_ranges[3] = CD3DX12_DESCRIPTOR_RANGE(
                            D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
                            _renderer->_material_texture_num,
                            0
                        );
                    }

                    // ルートパラメータを作成
                    // ディスクリプタヒープの対応表のディスクリプタテーブルの設定を作成
                    {
                        // ディスクリプタヒープの一つ目は[0]のレンジデータ
                        CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(root_params[0], 1, &desc_tbl_ranges[0]);
                        // ディスクリプタヒープの一つ目は[1]のレンジデータ
                        CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(root_params[1], 1, &desc_tbl_ranges[1]);
                        // ディスクリプタヒープの2つ目は[2] / [3]をひとまとめのレンジデータ
                        CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(root_params[2], 2, &desc_tbl_ranges[2]);
                    }

                    // ルートシグネチャー設定を作成
                    auto root_sig_desc = CD3DX12_ROOT_SIGNATURE_DESC(
                        // ルートパラメータの数
                        _countof(root_params),
                        // ルートパラメータ
                        root_params,
                        // サンプラーパラメータの数
                        _countof(sampler_descs),
                        // サンプラーパラメータ
                        sampler_descs,
                        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
                    );

                    ID3DBlob* p_error_blob = nullptr;

                    auto h_result = DirectX12::CreateRootSignature(this->_context, _renderer->_root_sig_key, &p_error_blob, root_sig_desc);
                    if (FAILED(h_result))
                    {
                        std::string error;
                        DirectX12::OutputErrorMessageForBlob(&error, h_result, p_error_blob);
                        LOGD << error.c_str();

                        assert(false);
                    }
                }
            }

            // グラフィックパイプラインを作成
            {
                // 頂点レイアウト作成
                D3D12_INPUT_ELEMENT_DESC input_layout[] = {
                    // 座標情報
                    {
                        // 予約セマンティック名
                        "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
                        D3D12_APPEND_ALIGNED_ELEMENT,
                        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
                    },

                    // 法線情報
                    {
                        // 予約セマンティック名
                        "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
                        D3D12_APPEND_ALIGNED_ELEMENT,
                        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
                    },

                    // uv情報
                    {
                        // 予約セマンティック名
                        "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
                        D3D12_APPEND_ALIGNED_ELEMENT,
                        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
                    },

                    // ボーン番号
                    {
                        // 独自セマンティック名
                        "BONE_NO", 0, DXGI_FORMAT_R16G16_UINT, 0,
                        D3D12_APPEND_ALIGNED_ELEMENT,
                        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
                    },

                    // ボーン影響度
                    {
                        // 独自セマンティック名
                        "WEIGHT", 0, DXGI_FORMAT_R8_UINT, 0,
                        D3D12_APPEND_ALIGNED_ELEMENT,
                        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
                    },

                    /*
                    // 輪郭線フラグ
                    {
                        // 独自セマンティック名
                        "EDGE_FLG", 0, DXGI_FORMAT_R8_UINT, 0,
                        D3D12_APPEND_ALIGNED_ELEMENT,
                        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
                    },
                    */
                };

                // あらかじめて作成したシェーダー, ルートシグネチャと頂点レイアウトを指定グラフィックパイプラインを作成
                auto h_result = DirectX12::CreateGraphicsPipeline(
                    this->_context,
                    _renderer->_gpipeline_key,
                    _renderer->_root_sig_key,
                    in_r_pmd_shader_vs_filepath,
                    in_r_pmd_shader_ps_filepath,
                    input_layout, _countof(input_layout));
            }

            LOGD << "end create renderer: " + in_r_pmd_filepath;

            return _renderer;
        }

        /// <summary>
        /// 指定したマテリアルデータを適応
        /// </summary>
        /// <param name="out_r_source"></param>
        /// <param name="in_r_file_path"></param>
        /// <param name="in_r_toon_file_path_fmt"></param>
        /// <param name="in_r_pmd_material"></param>
        void Factory::_ApplyMaterial(
            std::shared_ptr<Renderer> out_r_source,
            const std::string& in_r_current_resource_path,
            const std::string& in_r_toon_file_path_fmt,
            std::vector<PMD::Loader::PMDMaterial>& in_r_pmd_material)
        {
            out_r_source->_pmd_materials.resize(in_r_pmd_material.size());
            out_r_source->_pmd_textures.resize(in_r_pmd_material.size());

            for (size_t i = 0; i < out_r_source->_pmd_materials.size(); ++i)
            {
                out_r_source->_pmd_materials[i].indices_num = in_r_pmd_material[i].indices_num;
                out_r_source->_pmd_materials[i].basic.diffuse = in_r_pmd_material[i].diffuse;
                out_r_source->_pmd_materials[i].basic.alpha = in_r_pmd_material[i].alpha;
                out_r_source->_pmd_materials[i].basic.speclar = in_r_pmd_material[i].specular;
                out_r_source->_pmd_materials[i].basic.specularity = in_r_pmd_material[i].specularity;
                out_r_source->_pmd_materials[i].basic.ambient = in_r_pmd_material[i].ambiend;

                // リソースの初期化
                out_r_source->_pmd_textures[i].tex = this->_white_share_texture;
                out_r_source->_pmd_textures[i].sph = this->_white_share_texture;
                out_r_source->_pmd_textures[i].spa = this->_black_share_texture;
                out_r_source->_pmd_textures[i].toon = this->_gradation_share_texture;

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
                                    out_r_source->_pmd_textures[i].sph = DirectX12::CreateTextureResourceFromLoadTextureFile(this->_context, load_tex_file_path_str);
                                else if (Common::GetFileExtension(load_tex_file_path_str) == "spa")
                                    out_r_source->_pmd_textures[i].spa = DirectX12::CreateTextureResourceFromLoadTextureFile(this->_context, load_tex_file_path_str);
                                else
                                    out_r_source->_pmd_textures[i].tex = DirectX12::CreateTextureResourceFromLoadTextureFile(this->_context, load_tex_file_path_str);
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

                    out_r_source->_pmd_textures[i].toon = DirectX12::CreateTextureResourceFromLoadTextureFile(this->_context, toon_file_path);
                }
            }

            // マテリアルのディスクリプタ数
            out_r_source->_material_texture_num = PMD::Material::c_pmd_model_texture_num;
            out_r_source->_material_desc_num = 1 + out_r_source->_material_texture_num;
        }

        /// <summary>
        /// 指定したボーンデータを適応
        /// </summary>
        /// <param name="out_p_renderer"></param>
        /// <param name="in_r_pmd_bone"></param>
        void Factory::_ApplyBone(
            Renderer* out_p_source,
            std::vector<PMD::Loader::PMDBone>& in_r_pmd_bone)
        {
            // 親インデックスに対応したボーン名の配列
            std::vector<std::string> bone_names(in_r_pmd_bone.size());

            // ボーンテーブル作成
            {
                out_p_source->_bone_name_array.resize(in_r_pmd_bone.size());
                out_p_source->_bone_node_address_array.resize(in_r_pmd_bone.size());

                out_p_source->_knee_idxs.clear();
                for (size_t i = 0; i < in_r_pmd_bone.size(); ++i)
                {
                    auto& r_pmd_node = in_r_pmd_bone[i];

                    // 親インデックスに対応したボーン名を設定
                    bone_names[i] = r_pmd_node.bone_name;

                    // ボーン情報からノード情報を設定
                    auto& r_bone_node = out_p_source->_bone_node_table[r_pmd_node.bone_name];
                    r_bone_node.index = i;
                    r_bone_node.start_pos = r_pmd_node.pos;
                    r_bone_node.bone_type = static_cast<BoneType>(r_pmd_node.type);
                    r_bone_node.parent_bone = r_pmd_node.parent_no;
                    r_bone_node.ik_parent_bone = r_pmd_node.ik_bone_no;

                    // ボーンインデックスからボーン名を
                    // ボーンインデックスからボーンデータを
                    // すぐにアクセスできるようにするためのテーブルを用意
                    out_p_source->_bone_node_address_array[i] = &r_bone_node;
                    out_p_source->_bone_name_array[i] = bone_names[i];

                    // TODO: ひざの番号を収集する
                    {
                        std::string bone_name = r_pmd_node.bone_name;
                        if (bone_name.find("ひざ") != std::string::npos)
                        {
                            out_p_source->_knee_idxs.emplace_back(i);
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
                    out_p_source->_bone_node_table[parent_node_name].children.emplace_back(
                        &out_p_source->_bone_node_table[r_pmd_node.bone_name]
                    );
                }
            }

            // 各ボーンの行列作成
            auto& r_bone_matrices = out_p_source->_bone_matrices;
            r_bone_matrices.resize(in_r_pmd_bone.size());

            // すべて単位行列で初期化
            std::fill(r_bone_matrices.begin(), r_bone_matrices.end(), DirectX::XMMatrixIdentity());
        }

        /// <summary>
        /// IKのボーン情報をデバッグ出力
        /// </summary>
        /// <param name="in_r_renderer"></param>
        void Factory::_DebugPrintByIKBone(std::shared_ptr<Renderer> in_r_renderer, std::shared_ptr<::PMD::Loader::PMDDataPack> in_r_pmd_data_pack)
        {
            {
                // ボーンidxからボーン名を取得する関数変数をラムダ式で作る
                auto get_name_from_idx = [&](UINT16 idx)->std::string
                {
                    auto it = std::find_if(
                        in_r_renderer->_bone_node_table.begin(),
                        in_r_renderer->_bone_node_table.end(),
                        [idx](const std::pair<std::string, BoneNode>& obj)
                    {
                        return obj.second.index == idx;
                    }
                    );

                    if (it != in_r_renderer->_bone_node_table.end())
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
}