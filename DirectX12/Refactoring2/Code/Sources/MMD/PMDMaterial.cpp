#include "MMD/PMDMaterial.h"

#include "Common.h"
#include "DirectX12/DirectX12MiniHeader.h"

#include <tchar.h>

namespace PMD
{
    namespace Material
    {
        ShaderEffect::ShaderEffect()
        {
            this->_root_sig_key = "pmd_root_sig" + Common::CreateGUIDString();
            this->_gpipeline_key = "pmd_gpipeline" + Common::CreateGUIDString();

            this->_scene_desc_heap_share_key = "basic_heap" + Common::CreateGUIDString();
            this->_basic_buff_key = "pmd_scene_data" + Common::CreateGUIDString();

            this->_transform_buff_key = "transform_heap" + Common::CreateGUIDString();
            this->_transform_desc_heap_share_key = "pmd_transform_data" + Common::CreateGUIDString();

            this->_material_desc_heap_share_key = "pmd_material_heap" + Common::CreateGUIDString();
            this->_material_buff_key = "pmd_const_material" + Common::CreateGUIDString();
        }

        void ShaderEffect::Load(
            std::shared_ptr<DirectX12::Context> in_context,
            const std::string_view& in_r_vs_filepath,
            const std::string_view& in_r_ps_filepath)
        {
            this->_vs_filepath = in_r_vs_filepath;
            this->_ps_filepath = in_r_ps_filepath;

            // シェーダーロード
            {
                ID3D10Blob* out_p_error_blob = nullptr;
                HRESULT h_result;
                {
                    LOGD << "vs file load => " << this->_vs_filepath;

                    auto filepath = Common::ConvertStringToWideString(this->_vs_filepath);
                    h_result = DirectX12::LoadShaderVSBySync(in_context, this->_vs_filepath, &out_p_error_blob, filepath.c_str());
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
                    LOGD << "ps file load => " << this->_ps_filepath;

                    auto ps_file_path = Common::ConvertStringToWideString(this->_ps_filepath);
                    h_result = DirectX12::LoadShaderPSBySync(in_context, this->_ps_filepath, &out_p_error_blob, ps_file_path.c_str());
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
        }

        void ShaderEffect::Apply(
            std::shared_ptr<DirectX12::Context> in_context,
            std::vector<::PMD::Material::MaterialData>& in_r_pmd_materials,
            std::vector<::PMD::Material::MaterialTexture>& in_r_pmd_textures,
            // GPUに渡すボーン情報
            std::vector<DirectX::XMMATRIX>& in_r_bone_matrices)
        {
            // シェーダー効果適用に利用するので保存
            this->_pmd_materials = in_r_pmd_materials;

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

                    auto b_result = DirectX12::CreateDescripterHeap(in_context, this->_scene_desc_heap_share_key, desc_heap_desc);
                    assert(b_result);
                }

                // 座標変換ディスクリプタヒープ作成
                {
                    D3D12_DESCRIPTOR_HEAP_DESC desc_heap_desc = {};
                    desc_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                    desc_heap_desc.NodeMask = 0;
                    desc_heap_desc.NumDescriptors = 1;
                    desc_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

                    auto b_result = DirectX12::CreateDescripterHeap(in_context, this->_transform_desc_heap_share_key, desc_heap_desc);
                    assert(b_result);
                }
            }

            // マテリアルバッファを作成
            {
                DirectX12::Context::ComPtr<ID3D12Resource> p_material_buff;
                // 定数バッファありきで一旦考えてる
                // 定数バッファはデータ一塊が256byteになっている。
                // 定数バッファを使う場合は256byteのアライメントにしないと動かない作りにしている
                // なのでバッファに複数のデータを書き込む場合は256byteアライメントに調整しないといけない
                // 工夫すればメモリの無駄使いを解消できるみたいだが、ちょっと分からない
                size_t basic_size = sizeof(this->_pmd_materials[0].basic);
                size_t alignment_size = (basic_size + 0xff) & ~0xff;

                // マテリアル数分のバッファ作成してデータ書き込み
                {
                    // マテリアルバッファを作成
                    // マテリアルの数から256byteアライメントしたサイズで作成
                    // マテリアルの数 * アライメントサイズでバッファ作成している
                    p_material_buff = DirectX12::CreateEmptyResourceByGPUTransition(in_context, this->_material_buff_key, alignment_size * this->_pmd_materials.size());

                    // マテリアルデータを書き込み
                    INT8* p_map_material = nullptr;
                    auto result = p_material_buff->Map(0, nullptr, (void**)&p_map_material);
                    assert(SUCCEEDED(result));

                    for (auto& m : this->_pmd_materials)
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
                this->_material_desc_num = 0;
                {
                    // マテリアル定数バッファビューの設定データ
                    D3D12_CONSTANT_BUFFER_VIEW_DESC mat_desc = {};
                    {
                        mat_desc.BufferLocation = p_material_buff->GetGPUVirtualAddress();
                        // 定数バッファの一塊サイズ
                        mat_desc.SizeInBytes = alignment_size;
                    }
                    ++this->_material_desc_num;

                    // テクスチャバッファビューの設定データ
                    D3D12_SHADER_RESOURCE_VIEW_DESC tex_desc = {};
                    {
                        tex_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
                        tex_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                        tex_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                        tex_desc.Texture2D.MipLevels = 1;
                    }
                    ++this->_material_desc_num;

                    D3D12_SHADER_RESOURCE_VIEW_DESC sp_tex_multi_desc = {};
                    {
                        sp_tex_multi_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
                        sp_tex_multi_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                        sp_tex_multi_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                        sp_tex_multi_desc.Texture2D.MipLevels = 1;
                    }
                    ++this->_material_desc_num;

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
                    ++this->_material_desc_num;

                    D3D12_SHADER_RESOURCE_VIEW_DESC toon_tex_add_desc = {};
                    {
                        toon_tex_add_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
                        toon_tex_add_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                        toon_tex_add_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                        toon_tex_add_desc.Texture2D.MipLevels = 1;
                    }
                    ++this->_material_desc_num;

                    // ディスクリプタヒープとビューは密接に関わっているので一つでも設定をミスるとバグる
                    // マテリアルのデータ数 * (定数バッファ + テクスチャバッファ + スフィアテクスチャ + 加算スフィアテクスチャ)
                    {
                        // PMDマテリアル用のディスクリプタヒープ作成
                        // マテリアルの数が分かった段階でないと作れない
                        {
                            UINT32 desc_num = this->_pmd_materials.size() * this->_material_desc_num;
                            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
                            {
                                desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                                desc.NodeMask = 0;
                                desc.NumDescriptors = desc_num;
                                desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
                            }
                            auto b_result = DirectX12::CreateDescripterHeap(in_context, this->_material_desc_heap_share_key, desc);
                            assert(b_result);
                        }
                    }

                    // あらかじめて作成したディスクリプタヒープにビュー付け
                    auto heap = in_context->desc_heap_map[this->_material_desc_heap_share_key.c_str()];
                    assert(heap != nullptr);

                    auto heap_h = heap->GetCPUDescriptorHandleForHeapStart();
                    auto inc_size = in_context->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

                    // 一つのディスクリプタヒープに複数のマテリアルデータを展開している
                    // マテリアルデータ数分ビューを作成
                    for (size_t i = 0; i < this->_pmd_materials.size(); ++i)
                    {
                        // マテリアルの定数バッファビューを作成
                        in_context->dev->CreateConstantBufferView(&mat_desc, heap_h);
                        heap_h.ptr += inc_size;

                        // 設定するマテリアルのバッファアドレスを更新
                        mat_desc.BufferLocation += mat_desc.SizeInBytes;

                        // テクスチャをビューに追加
                        {
                            tex_desc.Format = in_r_pmd_textures[i].tex->GetDesc().Format;
                            // 事前に用意したテクスチャを設定してビューを作る
                            in_context->dev->CreateShaderResourceView(
                                in_r_pmd_textures[i].tex.Get(),
                                &tex_desc,
                                heap_h
                            );

                            heap_h.ptr += inc_size;
                        }

                        // スフィアテクスチャをビューに追加
                        {
                            sp_tex_multi_desc.Format = in_r_pmd_textures[i].sph->GetDesc().Format;
                            in_context->dev->CreateShaderResourceView(
                                in_r_pmd_textures[i].sph.Get(),
                                &sp_tex_multi_desc,
                                heap_h
                            );

                            heap_h.ptr += inc_size;
                        }

                        // 加算スフィアテクスチャをビューに追加
                        {
                            sp_tex_add_desc.Format = in_r_pmd_textures[i].spa->GetDesc().Format;
                            in_context->dev->CreateShaderResourceView(
                                in_r_pmd_textures[i].spa.Get(),
                                &sp_tex_add_desc,
                                heap_h
                            );

                            heap_h.ptr += inc_size;
                        }

                        // トゥーンテクスチャをビューに追加
                        {
                            toon_tex_add_desc.Format = in_r_pmd_textures[i].toon->GetDesc().Format;
                            in_context->dev->CreateShaderResourceView(
                                in_r_pmd_textures[i].toon.Get(),
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
                    auto buff = DirectX12::CreateEmptyResourceByGPUTransition(in_context, this->_basic_buff_key, (sizeof(SceneData) + 0xff) & ~0xff);
                    buff->Map(0, nullptr, (void**)&this->_p_scene_shader_param);
                    // マップ解除はしない
                    // 更新毎に書き込まれるから
                }

                // ディスクリプタヒープビュー作成
                {
                    auto heap = in_context->desc_heap_map[this->_scene_desc_heap_share_key.c_str()];
                    auto heap_handle = heap->GetCPUDescriptorHandleForHeapStart();
                    // 行列の定数バッファビューを作る
                    {
                        D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc = {};

                        auto buff = in_context->res_buff_map[this->_basic_buff_key.c_str()];
                        cbv_desc.BufferLocation = buff->GetGPUVirtualAddress();
                        LOGD << _T("basic_heap: buff->GetDesc().Width = ") << buff->GetDesc().Width;
                        cbv_desc.SizeInBytes = static_cast<UINT>(buff->GetDesc().Width);

                        // ディスクリプタヒープのハンドルにビュー作成
                        in_context->dev->CreateConstantBufferView(&cbv_desc, heap_handle);
                    }
                    // このディスクリプタヒープに対してはリソース一つのみ
                }
            }

            // 座標変換用の定数バッファ / ディスクリプタヒープを作成
            {
                size_t buff_size = sizeof(DirectX::XMMATRIX) * (in_r_bone_matrices.size() + 1);
                buff_size = (buff_size + 0xff) & ~0xff;
                {
                    // 座標変換とボーンのデータサイズ
                    auto buff = DirectX12::CreateEmptyResourceByGPUTransition(in_context, this->_transform_buff_key, buff_size);
                    buff->Map(0, nullptr, (void**)&this->_p_mapped_matrices);
                    // マップ解除はしない
                    // 更新毎に書き込まれるから

                    // 先頭はワールド行列
                    this->_p_mapped_matrices[0] = DirectX::XMMatrixIdentity();
                    // 以降はボーン行列
                    // コピー元と先で要素数が違うとバグるので注意
                    std::copy(in_r_bone_matrices.begin(), in_r_bone_matrices.end(), _p_mapped_matrices + 1);
                }

                // ディスクリプタヒープビュー作成
                {
                    auto heap = in_context->desc_heap_map[this->_transform_desc_heap_share_key.c_str()];
                    auto heap_handle = heap->GetCPUDescriptorHandleForHeapStart();
                    // 定数バッファビューを作る
                    {
                        D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc = {};

                        auto buff = in_context->res_buff_map[this->_transform_buff_key.c_str()];
                        cbv_desc.BufferLocation = buff->GetGPUVirtualAddress();
                        cbv_desc.SizeInBytes = buff_size;

                        in_context->dev->CreateConstantBufferView(&cbv_desc, heap_handle);
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
                            c_pmd_model_texture_num,
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

                    auto h_result = DirectX12::CreateRootSignature(in_context, this->_root_sig_key, &p_error_blob, root_sig_desc);
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
                    in_context,
                    this->_gpipeline_key,
                    this->_root_sig_key,
                    this->_vs_filepath,
                    this->_ps_filepath,
                    input_layout, _countof(input_layout));
            }
        }

        void ShaderEffect::BeginEffect(
            std::shared_ptr<DirectX12::Context> in_context)
        {
            // PMD用のパイプラインステートを設定
            {
                auto pipeline_state = in_context->_pipeline_state_map[this->_gpipeline_key.c_str()];
                in_context->cmd_list->SetPipelineState(pipeline_state.Get());
            }
        }

        void ShaderEffect::EndEffect(
            std::shared_ptr<DirectX12::Context> in_context)
        {
            // 利用するディスクリプタと関連付けたルートシグネチャを利用
            {
                auto root_sig = in_context->_root_sig_map[this->_root_sig_key.c_str()];
                in_context->cmd_list->SetGraphicsRootSignature(root_sig.Get());
            }

            // シーンパラメータを設定
            {
                auto p_basic_desc_heap = in_context->desc_heap_map[this->_scene_desc_heap_share_key.c_str()];
                assert(p_basic_desc_heap != nullptr);

                // ルートパラメータ0とディスクリプタヒープの関連付け
                in_context->cmd_list->SetDescriptorHeaps(1, p_basic_desc_heap.GetAddressOf());
                {
                    auto basic_heap_handle = p_basic_desc_heap->GetGPUDescriptorHandleForHeapStart();
                    in_context->cmd_list->SetGraphicsRootDescriptorTable(
                        // ルートパラメータインデックス
                        0,
                        // ヒープアドレス
                        basic_heap_handle);
                }
            }

            // 座標変換パラメータを設定
            {
                auto p_desc_heap = in_context->desc_heap_map[this->_transform_desc_heap_share_key.c_str()];
                assert(p_desc_heap != nullptr);

                // ルートパラメータ0とディスクリプタヒープの関連付け
                in_context->cmd_list->SetDescriptorHeaps(1, p_desc_heap.GetAddressOf());
                {
                    auto heap_handle = p_desc_heap->GetGPUDescriptorHandleForHeapStart();
                    in_context->cmd_list->SetGraphicsRootDescriptorTable(
                        // ルートパラメータインデックス
                        1,
                        // ヒープアドレス
                        heap_handle);
                }
            }

            // マテリアルのディスクリプタヒープ設置を使って描画コマンドを設定
            {
                auto p_desc_heap = in_context->desc_heap_map[this->_material_desc_heap_share_key.c_str()];
                in_context->cmd_list->SetDescriptorHeaps(1, p_desc_heap.GetAddressOf());
                {
                    // 一つのディスクリプタヒープにある複数ビューを参照するアドレスオフセットサイズ
                    auto cbvsrv_inc_size =
                        in_context->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * this->_material_desc_num;

                    auto material_h = p_desc_heap->GetGPUDescriptorHandleForHeapStart();
                    UINT32 idx_offset = 0;
                    for (auto& m : this->_pmd_materials)
                    {
                        in_context->cmd_list->SetGraphicsRootDescriptorTable(
                            // ルートパラメータインデックス
                            2,
                            // ヒープアドレス
                            material_h);

                        // 描画
                        in_context->cmd_list->DrawIndexedInstanced(m.indices_num, 1, idx_offset, 0, 0);

                        material_h.ptr += cbvsrv_inc_size;
                        idx_offset += m.indices_num;
                    }
                }
            }
        }
    }
}
