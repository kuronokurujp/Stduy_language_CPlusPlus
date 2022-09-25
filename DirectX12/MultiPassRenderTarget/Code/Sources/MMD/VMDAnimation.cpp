#include "MMD/VMDAnimation.h"

#include "Component/PMDMeshComponent.h"

#include "Common.h"

#include <algorithm>
#include <array>

namespace Animation
{
    VMDAnimation::VMDAnimation(const VMD::Loader::VMDDataPack& in_r_data_pack)
    {
        this->_ik_enables.resize(in_r_data_pack.ik_enables.size());
        std::copy(in_r_data_pack.ik_enables.begin(), in_r_data_pack.ik_enables.end(), this->_ik_enables.begin());

        for (auto& m : in_r_data_pack.motions)
        {
            DirectX::XMVECTOR q = DirectX::XMLoadFloat4(&m.quaternion);
            DirectX::XMFLOAT2 bezier_p1 = DirectX::XMFLOAT2(static_cast<float>(m.bezier[3]) / 127.0f, static_cast<float>(m.bezier[7] / 127.0f));
            DirectX::XMFLOAT2 bezier_p2 = DirectX::XMFLOAT2(static_cast<float>(m.bezier[11]) / 127.0f, static_cast<float>(m.bezier[15]) / 127.0f);

            auto item = MotionKeyFrame(m.frame_no, q, m.location, bezier_p1, bezier_p2);
            this->_motion_key_frames[m.bone_name].emplace_back(item);

            // モーション期間を取得
            this->_motion_duration = std::max<UINT32>(this->_motion_duration, item.frame_no);
        }

        // モーションキーフレームリストがフレーム番号で昇順で並べ替える
        for (auto& m : this->_motion_key_frames)
        {
            std::sort(m.second.begin(), m.second.end(),
                [](const MotionKeyFrame& in_r_first, const MotionKeyFrame& in_r_second)
            {
                return in_r_first.frame_no <= in_r_second.frame_no;
            });
        }
    }

    void VMDAnimation::PlayAnimation()
    {
        this->_start_time = ::timeGetTime();
    }

    void VMDAnimation::UpdateAnimation(Component::SkeltalMeshCameraComponent* in_p_mesh_comp)
    {
        // レンダリング情報を渡してアニメ更新
        auto mesh = dynamic_cast<Component::PMDMeshCameraComponent*>(in_p_mesh_comp);
        if (mesh == nullptr)
            return;

        this->_UpdateAnimation(mesh);
    }

    void VMDAnimation::_UpdateAnimation(Component::PMDMeshCameraComponent* in_p_comp)
    {
        auto elapsed_time = static_cast<float>(::timeGetTime() - this->_start_time);
        UINT32 now_frame_no = static_cast<UINT32>(30.0f * (elapsed_time / 1000.0f));

        // アニメをループさせる
        if (this->_motion_duration < now_frame_no)
        {
            this->_start_time = ::timeGetTime();
            now_frame_no = 0;
        }

        auto render_data = in_p_comp->GetBaseData();
        auto& bone_matrices = render_data->bone_matrices;
        std::fill(bone_matrices.begin(), bone_matrices.end(), DirectX::XMMatrixIdentity());

        for (auto& m : this->_motion_key_frames)
        {
            auto& bone_name = m.first;
            {
                auto find_it = render_data->bone_node_table.find(bone_name.c_str());
                if (find_it == render_data->bone_node_table.end())
                    continue;
            }

            auto& node = render_data->bone_node_table[bone_name.c_str()];
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
            auto node = render_data->bone_node_table[Component::PMDMeshCameraComponent::s_center_bone_name.c_str()];
            in_p_comp->MulBoneMatrixAndRecursive(&node, DirectX::XMMatrixIdentity());
        }

        // IK関連の処理はここで実行しないとだめ
        this->_IKSolve(in_p_comp, render_data, now_frame_no);

        // シェーダーに渡す行列更新
        std::copy(bone_matrices.begin(), bone_matrices.end(), in_p_comp->_shader_effect->GetMappedMatrices() + 1);
    }

    const float VMDAnimation::_CalcBezierByTwo2DPoint(
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
    void VMDAnimation::_IKSolve(
        Component::PMDMeshCameraComponent* in_p_comp,
        const std::shared_ptr<Component::PMDMeshCameraComponent::RenderBaseData> in_p_render_base_data, const UINT32 in_now_frame)
    {
        // IK有効フラグをチェックしてIK処理するか決める
        auto it = std::find_if(
            this->_ik_enables.rbegin(),
            this->_ik_enables.rend(),
            [in_now_frame](const VMD::Loader::VMDIkEnable& in_r_ik_enable) {
            return in_r_ik_enable.frame_no <= in_now_frame;
        });

        for (auto& ik : in_p_render_base_data->iks)
        {
            if (it != this->_ik_enables.rend())
            {
                auto ik_enable_it = it->ik_enable_table.find(in_p_render_base_data->bone_name_array[ik.bone_idx]);
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
                this->_SolveLockIK(in_p_render_base_data, ik);
                break;
            }
            case 2:
            {
                this->_SolveCosineIK(in_p_render_base_data, ik);
                break;
            }
            default:
                this->_SolveCCDIK(in_p_comp, in_p_render_base_data, ik);
                break;
            }
        }
    }

    void VMDAnimation::_SolveCCDIK(
        Component::PMDMeshCameraComponent* in_p_comp,
        const std::shared_ptr<Component::PMDMeshCameraComponent::RenderBaseData> in_p_render_base_data, const PMD::Loader::PMDIK& in_r_ik)
    {
        // ターゲット
        auto target_bone_node = in_p_render_base_data->bone_node_address_array[in_r_ik.bone_idx];
        auto target_org_pos = DirectX::XMLoadFloat3(&target_bone_node->start_pos);

        // ターゲット座標をワールドからローカル座標系に変換
        const DirectX::XMMATRIX parent_mat = in_p_render_base_data->bone_matrices[in_p_render_base_data->bone_node_address_array[in_r_ik.bone_idx]->ik_parent_bone];
        DirectX::XMVECTOR det;
        auto inv_parent_mat = DirectX::XMMatrixInverse(&det, parent_mat);
        auto target_next_pos = DirectX::XMVector3Transform(
            target_org_pos, in_p_render_base_data->bone_matrices[in_r_ik.bone_idx] * inv_parent_mat);

        // 末端ノード
        auto end_pos = DirectX::XMLoadFloat3(&in_p_render_base_data->bone_node_address_array[in_r_ik.target_idx]->start_pos);

        // 中間ノード(ルートを含む)
        std::vector<DirectX::XMVECTOR> bone_positions;
        {
            for (auto& cidx : in_r_ik.node_idxs)
            {
                bone_positions.push_back(DirectX::XMLoadFloat3(&in_p_render_base_data->bone_node_address_array[cidx]->start_pos));
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
                in_p_render_base_data->bone_matrices[cidx] = mats[idx];
                ++idx;
            }
        }

        // ボーンの親子関係更新
        {
            auto root_node = in_p_render_base_data->bone_node_address_array[in_r_ik.node_idxs.back()];
            in_p_comp->MulBoneMatrixAndRecursive(root_node, parent_mat, true);
        }
    }

    void VMDAnimation::_SolveCosineIK(
        const std::shared_ptr<Component::PMDMeshCameraComponent::RenderBaseData> in_p_render_base_data, const PMD::Loader::PMDIK& in_r_ik)
    {
        // IK構成点を保存
        std::vector<DirectX::XMVECTOR> positions;
        // IKのそれぞれのボーン間の距離を保存
        std::array<float, 2> edge_lens;

        // 末端ボーンが近づく目標ボーン
        auto& target_node = in_p_render_base_data->bone_node_address_array[in_r_ik.bone_idx];
        auto target_pos = DirectX::XMVector3Transform(
            DirectX::XMLoadFloat3(&target_node->start_pos), in_p_render_base_data->bone_matrices[in_r_ik.bone_idx]);

        // IKチェーンが逆順らしいので、逆に並べる
        // 末端ボーン
        {
            auto end_node = in_p_render_base_data->bone_node_address_array[in_r_ik.target_idx];
            positions.emplace_back(DirectX::XMLoadFloat3(&end_node->start_pos));
        }

        // 中間 or ルートボーン
        {
            for (auto& chain_bone_idx : in_r_ik.node_idxs)
            {
                auto bone_node = in_p_render_base_data->bone_node_address_array[chain_bone_idx];
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
            positions[0] = DirectX::XMVector3Transform(positions[0], in_p_render_base_data->bone_matrices[in_r_ik.node_idxs[1]]);
            // 先端ボーン
            positions[2] = DirectX::XMVector3Transform(positions[2], in_p_render_base_data->bone_matrices[in_r_ik.bone_idx]);
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
                in_p_render_base_data->knee_idxs.begin(),
                in_p_render_base_data->knee_idxs.end(),
                in_r_ik.node_idxs[0]) == in_p_render_base_data->knee_idxs.end())
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

            in_p_render_base_data->bone_matrices[in_r_ik.node_idxs[1]] *= mat1;
            in_p_render_base_data->bone_matrices[in_r_ik.node_idxs[0]] = mat2 * in_p_render_base_data->bone_matrices[in_r_ik.node_idxs[1]];
            in_p_render_base_data->bone_matrices[in_r_ik.target_idx] = in_p_render_base_data->bone_matrices[in_r_ik.node_idxs[0]];
        }
    }

    void VMDAnimation::_SolveLockIK(
        const std::shared_ptr<Component::PMDMeshCameraComponent::RenderBaseData> in_p_render_base_data, const PMD::Loader::PMDIK& in_r_ik)
    {
        // 制御するノードが一つしかないので配列要素の直接指定でいい
        auto root_node = in_p_render_base_data->bone_node_address_array[in_r_ik.node_idxs[0]];
        auto target_node = in_p_render_base_data->bone_node_address_array[in_r_ik.target_idx];

        auto rpos1 = DirectX::XMLoadFloat3(&root_node->start_pos);
        auto tpos1 = DirectX::XMLoadFloat3(&target_node->start_pos);

        // ノード先となるボーンの座標を取得
        auto rpos2 = DirectX::XMVector3Transform(rpos1, in_p_render_base_data->bone_matrices[in_r_ik.node_idxs[0]]);
        auto tpos2 = DirectX::XMVector3Transform(tpos1, in_p_render_base_data->bone_matrices[in_r_ik.bone_idx]);

        // ノードと目的のボーンとの方向ベクトルを取得
        auto origin_vec = DirectX::XMVectorSubtract(tpos1, rpos1);
        auto target_vec = DirectX::XMVectorSubtract(tpos2, rpos2);

        origin_vec = DirectX::XMVector3Normalize(origin_vec);
        target_vec = DirectX::XMVector3Normalize(target_vec);

        DirectX::XMMATRIX mat = DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorScale(rpos2, -1)) *
            DirectX12::LockAtMatrix(origin_vec, target_vec, DirectX12::XFloat3UP, DirectX12::XFloat3Right) *
            DirectX::XMMatrixTranslationFromVector(rpos2);

        // 方向ベクトルに従ってボーンを回転させる
        in_p_render_base_data->bone_matrices[in_r_ik.node_idxs[0]] = mat;
    }
}
