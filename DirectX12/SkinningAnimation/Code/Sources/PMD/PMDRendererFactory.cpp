#include "PMD/PMDRendererFactory.h"

#include "PMD/PMDLoader.h"

#include "Common.h"

#include <tchar.h>

namespace PMD
{
    namespace Render
    {
        void Renderer::RecursiveMatrixMuliply(
            BoneNode* in_p_node, const DirectX::XMMATRIX& in_r_mat)
        {
            this->_bone_matrices[in_p_node->index] *= in_r_mat;

            for (auto& r_node : in_p_node->children)
            {
                this->RecursiveMatrixMuliply(r_node, this->_bone_matrices[in_p_node->index]);
            }
        }

        /// <summary>
        /// レンダリング
        /// </summary>
        /// <param name="in_r_local_mat"></param>
        /// <param name="in_r_world_mat"></param>
        /// <param name="in_r_view_mat"></param>
        /// <param name="in_r_proj_mat"></param>
        /// <param name="in_r_cam_pos"></param>
        void Renderer::Rendering(
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

            // 空バッファに頂点ビュー定義
            D3D12_VERTEX_BUFFER_VIEW vb_view = {};
            // 頂点バッファ構成を出力
            // 毎フレームやっても問題ない？
            // 頂点バッファが変わった時は当然ビューも変わるので更新箇所でやるのがいい気が
            {
                // 内容を決めるバッファを取得
                auto buff = this->_context->res_buff_map[this->_vs_buff_key.c_str()];
                vb_view.BufferLocation = buff->GetGPUVirtualAddress();
                // バッファ全サイズ
                vb_view.SizeInBytes = this->_vb_size_in_bytes;
                // バッファ内の一塊となるデータサイズ
                vb_view.StrideInBytes = this->_vb_stride_in_bytes;
            }

            // 空バッファにインデックスビュー定義
            D3D12_INDEX_BUFFER_VIEW ib_view = {};
            {
                // 内容を決めるバッファを取得
                auto buff = this->_context->res_buff_map[this->_idx_buff_key.c_str()];
                ib_view.BufferLocation = buff->GetGPUVirtualAddress();
                // インデックスの要素データ型を16byteにしている
                ib_view.Format = DXGI_FORMAT_R16_UINT;
                ib_view.SizeInBytes = this->_id_size_in_bytes;
            }

            // 描画
            this->_context->cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            // 一度に設定できるインデックスバッファビューは一つのみ
            this->_context->cmd_list->IASetIndexBuffer(&ib_view);
            // 頂点バッファビューを設定
            this->_context->cmd_list->IASetVertexBuffers(0, 1, &vb_view);

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
        void Factory::Termiate()
        {
            this->_white_share_texture.Reset();
            this->_black_share_texture.Reset();
            this->_gradation_share_texture.Reset();
        }

        /// <summary>
        /// PMDファイルを解析してレンダラー作成
        /// </summary>
        /// <param name="in_r_context"></param>
        /// <param name="in_r_key"></param>
        /// <param name="in_r_pmd_filepath"></param>
        /// <param name="in_r_pmd_shader_vs_filepath"></param>
        /// <param name="in_r_pmd_shader_ps_filepath"></param>
        /// <returns></returns>
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
                    auto catch_data = this->_data_pack_map.find(in_r_pmd_filepath.c_str());
                    if (catch_data == this->_data_pack_map.end())
                    {
                        // PMDファイルロード
                        auto error = PMD::Loader::SyncLoadFile(
                            &pmd_data_pack,
                            in_r_pmd_filepath.c_str());
                        assert(error == 0);

                        this->_data_pack_map[in_r_pmd_filepath.c_str()] = pmd_data_pack;
                    }
                    else
                    {
                        pmd_data_pack = catch_data->second;
                    }
                }

                // マテリアルデータ構築
                this->_ApplyRenderMaterialData(_renderer, in_r_pmd_filepath, in_r_toon_path_fmt, pmd_data_pack.material);

                // TODO: ボーンデータ構築
                this->_ApplyRenderBoneData(_renderer.get(), pmd_data_pack.bone);
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

                    auto filepath = Common::GetWideStringFromString(in_r_pmd_shader_vs_filepath);
                    h_result = DirectX12::SyncLoadShaderVS(this->_context, in_r_pmd_shader_vs_filepath, &out_p_error_blob, filepath.c_str());
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

                    auto ps_file_path = Common::GetWideStringFromString(in_r_pmd_shader_ps_filepath);
                    h_result = DirectX12::SyncLoadShaderPS(this->_context, in_r_pmd_shader_ps_filepath, &out_p_error_blob, ps_file_path.c_str());
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

            // 頂点データ定義
            {
                // 頂点バッファビュー作成
                LOGD << _T("create v buffer");
                {
                    _renderer->_vb_size_in_bytes = pmd_data_pack.vertexs.size() * sizeof(pmd_data_pack.vertexs[0]);
                    _renderer->_vb_stride_in_bytes = pmd_data_pack.vertex_size;

                    // 頂点情報を書き込むバッファを作成
                    auto p_vert_buff = DirectX12::CreateBlankResForHeapUpload(this->_context, _renderer->_vs_buff_key, _renderer->_vb_size_in_bytes);
                    assert(p_vert_buff != nullptr);

                    // 作った頂点バッファに情報をコピーする
                    // この書き込み方法は共通化できないので個別で行う
                    // コピーデータを一括して書き込む
                    {
                        UINT8* p_vert_map = nullptr;
                        auto result = p_vert_buff->Map(0, nullptr, (void**)&p_vert_map);
                        assert(SUCCEEDED(result));

                        // 一括データ転送
                        std::copy(pmd_data_pack.vertexs.begin(), pmd_data_pack.vertexs.end(), p_vert_map);

                        p_vert_buff->Unmap(0, nullptr);
                    }
                }

                // 頂点のインデックスバッファビューを作成
                LOGD << _T("create idx buffer");
                {
                    // インデックスバッファを作成
                    _renderer->_id_size_in_bytes = pmd_data_pack.indices.size() * sizeof(pmd_data_pack.indices[0]);
                    {
                        auto p_idx_buff = DirectX12::CreateBlankResForHeapUpload(this->_context, _renderer->_idx_buff_key, _renderer->_id_size_in_bytes);
                        assert(p_idx_buff != nullptr);

                        // コピーするデータ先頭と末尾の型と転送する型を指定
                        UINT16* p_map = nullptr;
                        auto result = p_idx_buff->Map(0, nullptr, (void**)&p_map);
                        assert(SUCCEEDED(result));

                        std::copy(pmd_data_pack.indices.begin(), pmd_data_pack.indices.end(), p_map);

                        p_idx_buff->Unmap(0, nullptr);
                    }
                }
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
                    p_material_buff = DirectX12::CreateBlankResForHeapUpload(this->_context, _renderer->_material_buff_key, alignment_size * _renderer->_pmd_materials.size());

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
                    // 一文字違いの変数だとミスとなる
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
                    auto buff = DirectX12::CreateBlankResForHeapUpload(this->_context, _renderer->_basic_buff_key, (sizeof(SceneShaderData) + 0xff) & ~0xff);
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
                    auto buff = DirectX12::CreateBlankResForHeapUpload(this->_context, _renderer->_transform_buff_key, buff_size);
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

        void Factory::_ApplyRenderMaterialData(
            std::shared_ptr<Renderer> out_r_renderer,
            const std::string& in_r_file_path,
            const std::string& in_r_toon_file_path_fmt,
            std::vector<PMD::Loader::PMDMaterial>& in_r_pmd_material)
        {
            out_r_renderer->_pmd_materials.resize(in_r_pmd_material.size());
            out_r_renderer->_pmd_textures.resize(in_r_pmd_material.size());

            for (size_t i = 0; i < out_r_renderer->_pmd_materials.size(); ++i)
            {
                out_r_renderer->_pmd_materials[i].indices_num = in_r_pmd_material[i].indices_num;
                out_r_renderer->_pmd_materials[i].basic.diffuse = in_r_pmd_material[i].diffuse;
                out_r_renderer->_pmd_materials[i].basic.alpha = in_r_pmd_material[i].alpha;
                out_r_renderer->_pmd_materials[i].basic.speclar = in_r_pmd_material[i].specular;
                out_r_renderer->_pmd_materials[i].basic.specularity = in_r_pmd_material[i].specularity;
                out_r_renderer->_pmd_materials[i].basic.ambient = in_r_pmd_material[i].ambiend;

                // リソースの初期化
                out_r_renderer->_pmd_textures[i].tex = this->_white_share_texture;
                out_r_renderer->_pmd_textures[i].sph = this->_white_share_texture;
                out_r_renderer->_pmd_textures[i].spa = this->_black_share_texture;
                out_r_renderer->_pmd_textures[i].toon = this->_gradation_share_texture;

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
                            // ロードする相対ファイルパスを作成
                            auto load_tex_file_path_str =
                                Common::GetPathForAddPathToDirectoryEndPath(in_r_file_path, load_file_path.c_str());

                            // ロードするテクスチャファイルパス
                            {
                                if (Common::GetFileExtension(load_tex_file_path_str) == "sph")
                                    out_r_renderer->_pmd_textures[i].sph = DirectX12::CreateTextureResourceFromLoadTextureFile(this->_context, load_tex_file_path_str);
                                else if (Common::GetFileExtension(load_tex_file_path_str) == "spa")
                                    out_r_renderer->_pmd_textures[i].spa = DirectX12::CreateTextureResourceFromLoadTextureFile(this->_context, load_tex_file_path_str);
                                else
                                    out_r_renderer->_pmd_textures[i].tex = DirectX12::CreateTextureResourceFromLoadTextureFile(this->_context, load_tex_file_path_str);
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

                    out_r_renderer->_pmd_textures[i].toon = DirectX12::CreateTextureResourceFromLoadTextureFile(this->_context, toon_file_path);
                }
            }

            // マテリアルのディスクリプタ数
            out_r_renderer->_material_texture_num = PMD::Material::c_pmd_model_texture_num;
            out_r_renderer->_material_desc_num = 1 + out_r_renderer->_material_texture_num;
        }

        /// <summary>
        /// ボーンデータ構築
        /// </summary>
        /// <param name="out_p_renderer"></param>
        /// <param name="in_r_pmd_bone"></param>
        void Factory::_ApplyRenderBoneData(
            Renderer* out_p_renderer,
            std::vector<PMD::Loader::PMDBone>& in_r_pmd_bone)
        {
            // 親インデックスに対応したボーン名の配列
            std::vector<std::string> bone_names(in_r_pmd_bone.size());

            // ボーンテーブル作成
            {
                for (size_t i = 0; i < in_r_pmd_bone.size(); ++i)
                {
                    auto& r_pmd_node = in_r_pmd_bone[i];

                    // 親インデックスに対応したボーン名を設定
                    bone_names[i] = r_pmd_node.bone_name;

                    // ボーン情報からノード情報を設定
                    auto& r_bone_node = out_p_renderer->_bone_node_table[r_pmd_node.bone_name];
                    r_bone_node.index = i;
                    r_bone_node.start_pos = r_pmd_node.pos;
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
                    out_p_renderer->_bone_node_table[parent_node_name].children.emplace_back(
                        &out_p_renderer->_bone_node_table[r_pmd_node.bone_name]
                    );
                }
            }

            // 各ボーンの行列作成
            auto& r_bone_matrices = out_p_renderer->_bone_matrices;
            r_bone_matrices.resize(in_r_pmd_bone.size());

            // すべて単位行列で初期化
            std::fill(r_bone_matrices.begin(), r_bone_matrices.end(), DirectX::XMMatrixIdentity());
        }
    }
}
