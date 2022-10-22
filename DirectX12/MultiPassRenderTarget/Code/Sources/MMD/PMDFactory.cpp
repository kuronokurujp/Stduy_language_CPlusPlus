#include "MMD/PMDFactory.h"

#include "MMD/PMDLoader.h"
#include "Component/PMDMeshComponent.h"

#include "Common.h"

#include <tchar.h>

namespace PMD
{
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
            Component::PMDMeshCameraComponent::RenderBaseData* out_p_source,
            std::shared_ptr<DirectX12::Context> in_dx12_context,
            const std::string& in_r_current_resource_path,
            const std::string& in_r_toon_file_path_fmt,
            std::vector<PMD::Loader::PMDMaterial>& in_r_pmd_material,
            DirectX12::ComPtr<ID3D12Resource> in_white_share_texture,
            DirectX12::ComPtr<ID3D12Resource> in_black_share_texture,
            DirectX12::ComPtr<ID3D12Resource> in_gradation_share_texture)
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
                                LOGD << _T("load texture => ") << load_tex_file_path_str;

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
            Component::PMDMeshCameraComponent::RenderBaseData* out_p_source,
            std::vector<PMD::Loader::PMDBone>& in_r_pmd_bone)
        {
            // 親インデックスに対応したボーン名の配列
            std::vector<std::string> bone_names(in_r_pmd_bone.size());

            // ボーンテーブル作成
            {
                out_p_source->bone_name_array.resize(in_r_pmd_bone.size());
                out_p_source->bone_node_address_array.resize(in_r_pmd_bone.size());

                out_p_source->knee_idxs.clear();
                auto size = static_cast<UINT32>(in_r_pmd_bone.size());
                for (UINT32 i = 0; i < size; ++i)
                {
                    auto& r_pmd_node = in_r_pmd_bone[i];

                    // 親インデックスに対応したボーン名を設定
                    bone_names[i] = r_pmd_node.bone_name;

                    // ボーン情報からノード情報を設定
                    auto& r_bone_node = out_p_source->bone_node_table[r_pmd_node.bone_name];
                    r_bone_node.index = i;
                    r_bone_node.start_pos = r_pmd_node.pos;
                    r_bone_node.bone_type = static_cast<Component::PMDMeshCameraComponent::BoneType>(r_pmd_node.type);
                    r_bone_node.parent_bone = r_pmd_node.parent_no;
                    r_bone_node.ik_parent_bone = r_pmd_node.ik_bone_no;

                    // ボーンインデックスからボーン名を
                    // ボーンインデックスからボーンデータを
                    // すぐにアクセスできるようにするためのテーブルを用意
                    out_p_source->bone_node_address_array[i] = &r_bone_node;
                    out_p_source->bone_name_array[i] = bone_names[i];

                    // ひざの番号を収集する
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
        void _DebugPrintByIKBone(Component::PMDMeshCameraComponent::RenderBaseData& in_r_data, std::shared_ptr<::PMD::Loader::PMDDataPack> in_r_pmd_data_pack)
        {
            {
                // ボーンidxからボーン名を取得する関数変数をラムダ式で作る
                auto get_name_from_idx = [&](UINT16 idx)->std::string
                {
                    auto it = std::find_if(
                        in_r_data.bone_node_table.begin(),
                        in_r_data.bone_node_table.end(),
                        [idx](const std::pair<std::string, Component::PMDMeshCameraComponent::BoneNode>& obj)
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
    std::shared_ptr<Animation::BaseAnimation> Factory::CreateAnimation(
        const std::string& in_r_pmd_filepath)
    {
        VMD::Loader::VMDDataPack pack;
        VMD::Loader::LoadFileBySync(&pack, in_r_pmd_filepath.c_str());

        std::shared_ptr<Animation::BaseAnimation> motion = std::make_shared<Animation::VMDAnimation>(pack);
        return motion;
    }

    /// <summary>
    /// PMDファイルをロードしてレンダリングデータを生成して返す
    /// </summary>
    /// <param name="out_p_render_data"></param>
    /// <param name="in_r_pmd_filepath"></param>
    /// <param name="in_r_toontex_filepath"></param>
    /// <returns></returns>
    const bool Factory::LoadRenderData(
        std::shared_ptr<Component::PMDMeshCameraComponent::RenderBaseData> out_p_render_data,
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
            else
            {
                pmd_data_pack = catch_data->second;
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
    /// レンダリングデータからPMDメッシュコンポーネントを生成
    /// </summary>
    std::shared_ptr<Component::PMDMeshCameraComponent> Factory::CreateMeshComponent(
        Actor::Actor* in_p_actor,
        std::shared_ptr<Component::PMDMeshCameraComponent::RenderBaseData> in_p_render_data,
        const std::string& in_r_pmd_shader_vs_filepath,
        const std::string& in_r_pmd_shader_ps_filepath)
    {
        std::shared_ptr<Component::PMDMeshCameraComponent> comp =
            std::make_shared<Component::PMDMeshCameraComponent>(
                in_p_actor,
                this->_context,
                in_p_render_data,
                in_r_pmd_shader_vs_filepath,
                in_r_pmd_shader_ps_filepath);

        return comp;
    }
}
