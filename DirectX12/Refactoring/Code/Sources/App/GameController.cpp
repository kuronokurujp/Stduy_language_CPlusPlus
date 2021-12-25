#include "App/GameController.h"

#include "Common.h"
#include "PMD/PMDMaterial.h"

#include <tchar.h>

namespace App
{
    GameController::GameController(std::shared_ptr<GUI::DirectX12WindowController> in_window_ctrl)
    {
        this->_window_ctrl = in_window_ctrl;
    }

    const bool GameController::Start()
    {
        std::shared_ptr<DirectX12::Context> context;
        {
            auto model = this->_window_ctrl->GetModel();
            context = model->Context();
        }

        // PMDファイルを開く
        std::vector<PMD::Loader::PMDMaterial> pmd_material;
        {
            const char* p_model_path = "Resources/Model/Miku.pmd";
            auto error = PMD::Loader::SyncLoadFile(&this->_pmd_data_pack, &pmd_material, p_model_path);
            assert(error == 0);

            // マテリアルデータコピー
            {
                // nullptr用のテクスチャを作成
                // これは共通利用する
                DirectX12::Context::ComPtr<ID3D12Resource> white_share_texture;
                DirectX12::Context::ComPtr<ID3D12Resource> black_share_texture;
                DirectX12::Context::ComPtr<ID3D12Resource> gradation_share_texture;
                {
                    white_share_texture = DirectX12::CreateTextureResourceFromColor(context, 0xff);
                    black_share_texture = DirectX12::CreateTextureResourceFromColor(context, 0x00);
                    gradation_share_texture = DirectX12::CreateTextureResourceFromGradation(context, 0xff);
                }

                this->_pmd_materials.resize(pmd_material.size());
                this->_pmd_textures.resize(pmd_material.size());

                for (size_t i = 0; i < this->_pmd_materials.size(); ++i)
                {
                    this->_pmd_materials[i].indices_num = pmd_material[i].indices_num;
                    this->_pmd_materials[i].basic.diffuse = pmd_material[i].diffuse;
                    this->_pmd_materials[i].basic.alpha = pmd_material[i].alpha;
                    this->_pmd_materials[i].basic.speclar = pmd_material[i].specular;
                    this->_pmd_materials[i].basic.specularity = pmd_material[i].specularity;
                    this->_pmd_materials[i].basic.ambient = pmd_material[i].ambiend;

                    // リソースの初期化
                    this->_pmd_textures[i].tex = white_share_texture;
                    this->_pmd_textures[i].sph = white_share_texture;
                    this->_pmd_textures[i].spa = black_share_texture;
                    this->_pmd_textures[i].toon = gradation_share_texture;

                    // テクスチャファイルパスからロードするファイルパスに置き換えてテクスチャロード
                    // マテリアルの数分用意
                    {
                        std::vector<std::string> load_file_paths;
                        if (0 < std::strlen(pmd_material[i].tex_file_path))
                        {
                            load_file_paths.clear();

                            auto tex_file_path_str = std::string(pmd_material[i].tex_file_path);
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
                                auto load_tex_file_path_str = Common::GetPathForAddPathToDirectoryEndPath(p_model_path, load_file_path.c_str());

                                // ロードするテクスチャファイルパス
                                {
                                    if (Common::GetFileExtension(load_tex_file_path_str) == "sph")
                                        this->_pmd_textures[i].sph = DirectX12::CreateTextureResourceFromLoadTextureFile(context, load_tex_file_path_str);
                                    else if (Common::GetFileExtension(load_tex_file_path_str) == "spa")
                                        this->_pmd_textures[i].spa = DirectX12::CreateTextureResourceFromLoadTextureFile(context, load_tex_file_path_str);
                                    else
                                        this->_pmd_textures[i].tex = DirectX12::CreateTextureResourceFromLoadTextureFile(context, load_tex_file_path_str);
                                }
                            }
                        }
                    }

                    // トゥーン番号からトゥーンテクスチャをロード
                    {
                        std::string toon_file_path;
                        // トゥーンテクスチャファイルパスを生成
                        {
                            const char* fmt = "Resources/Model/Toon/toon%02d.bmp";

                            UINT32 file_toon_no = pmd_material[i].toon_idx + 1;
                            int sz = std::snprintf(nullptr, 0, fmt, file_toon_no);
                            toon_file_path.resize(sz + 1);
                            std::snprintf(&toon_file_path[0], toon_file_path.size(), fmt, file_toon_no);
                        }

                        this->_pmd_textures[i].toon = DirectX12::CreateTextureResourceFromLoadTextureFile(context, toon_file_path);
                    }
                }
            }

            // マテリアルのディスクリプタ数
            this->_material_texture_num = PMD::Material::c_pmd_model_texture_num;
            this->_material_desc_num = 1 + this->_material_texture_num;
        }

        // ティスクリプタヒープを作る
        // 今回は2つ作る
        // しかしマザボのOnGPUだとディスクリプタヒープは1つしか作れないので注意
        {
            // 基本ディスクリプタヒープ作成
            {
                D3D12_DESCRIPTOR_HEAP_DESC desc_heap_desc = {};
                desc_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                desc_heap_desc.NodeMask = 0;
                desc_heap_desc.NumDescriptors = 1;
                desc_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

                auto b_result = DirectX12::CreateDescripterHeap(context, "basic_heap", desc_heap_desc);
                assert(b_result);
            }

            // PMDマテリアル用のディスクリプタヒープ作成
            // マテリアルの数が分かった段階でないと作れない
            {
                // ディスクリプタヒープとビューは密接に関わっているので一つでも設定をミスるとバグる
                // 定数バッファ + (テクスチャバッファ + スフィアテクスチャ + 加算スフィアテクスチャ)
                UINT32 desc_num = this->_pmd_materials.size() * this->_material_desc_num;
                ID3D12DescriptorHeap* p_material_heap = nullptr;

                D3D12_DESCRIPTOR_HEAP_DESC desc = {};
                {
                    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                    desc.NodeMask = 0;
                    desc.NumDescriptors = desc_num;
                    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
                }
                auto b_result = DirectX12::CreateDescripterHeap(context, "pmd_material_heap", desc);
                assert(b_result);
            }
        }

        // シェーダーロード
        {
            ID3D10Blob* out_p_error_blob = nullptr;
            HRESULT h_result;
            {
                auto vs_file_path = _T("Resources/Shader/PMD/BasicVertexShader.hlsl");
                LOGD << _T("vs file load => ") << vs_file_path;

                h_result = DirectX12::SyncLoadShaderVS(context, "pmd_vs", &out_p_error_blob, vs_file_path);
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
                auto ps_file_path = _T("Resources/Shader/PMD/BasicPixelShader.hlsl");
                LOGD << _T("ps file load => ") << ps_file_path;

                h_result = DirectX12::SyncLoadShaderPS(context, "pmd_ps", &out_p_error_blob, ps_file_path);
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
                UINT buff_memory_size = this->_pmd_data_pack.vertexs.size() * sizeof(this->_pmd_data_pack.vertexs[0]);

                // 頂点情報を書き込むバッファを作成
                auto p_vert_buff = DirectX12::CreateBlankResForHeapUpload(context, "pmd_vs_buff", buff_memory_size);
                assert(p_vert_buff != nullptr);

                // 作った頂点バッファに情報をコピーする
                // この書き込み方法は共通化できないので個別で行う
                // コピーデータを一括して書き込む
                {
                    UINT8* p_vert_map = nullptr;
                    auto result = p_vert_buff->Map(0, nullptr, (void**)&p_vert_map);
                    assert(SUCCEEDED(result));

                    // 一括データ転送
                    std::copy(this->_pmd_data_pack.vertexs.begin(), this->_pmd_data_pack.vertexs.end(), p_vert_map);

                    p_vert_buff->Unmap(0, nullptr);
                }
            }

            // 頂点のインデックスバッファビューを作成
            LOGD << _T("create idx buffer");
            {
                // インデックスバッファを作成
                UINT buff_memory_size =
                    static_cast<UINT>(this->_pmd_data_pack.indices.size()) * sizeof(this->_pmd_data_pack.indices[0]);
                {
                    auto p_idx_buff = DirectX12::CreateBlankResForHeapUpload(context, "pmd_idx_buff", buff_memory_size);
                    assert(p_idx_buff != nullptr);

                    // コピーするデータ先頭と末尾の型と転送する型を指定
                    UINT16* p_map = nullptr;
                    auto result = p_idx_buff->Map(0, nullptr, (void**)&p_map);
                    assert(SUCCEEDED(result));

                    std::copy(this->_pmd_data_pack.indices.begin(), this->_pmd_data_pack.indices.end(), p_map);

                    p_idx_buff->Unmap(0, nullptr);
                }
            }
        }

        // マテリアル用のビュー作成
        {
            // マテリアルバッファを作成
            DirectX12::Context::ComPtr<ID3D12Resource> p_material_buff;
            // 定数バッファはデータ一塊が256byteになっている。
            // なのでバッファに複数のデータを書き込む場合は256byteアライメントに調整しないといけない
            // 工夫すればメモリの無駄使いを解消できるみたいだが、ちょっと分からない
            size_t basic_size = sizeof(this->_pmd_materials[0].basic);
            size_t alignment_size = (basic_size + 0xff) & ~0xff;

            // バッファ作成とバッファにデータ書き込み
            {
                // マテリアルバッファを作成
                // マテリアルの数から256byteアライメントしたサイズで作成
                // マテリアルの数 * アライメントサイズでバッファ作成している
                p_material_buff = DirectX12::CreateBlankResForHeapUpload(context, "pmd_const_material", alignment_size * this->_pmd_materials.size());

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
                auto heap = context->desc_heap_map["pmd_material_heap"];
                assert(heap != nullptr);

                auto heap_h = heap->GetCPUDescriptorHandleForHeapStart();
                auto inc_size = context->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

                // マテリアルデータ数分ビューを作成
                for (size_t i = 0; i < this->_pmd_materials.size(); ++i)
                {
                    // マテリアルの定数バッファビューを作成
                    context->dev->CreateConstantBufferView(&mat_desc, heap_h);
                    heap_h.ptr += inc_size;

                    // 設定するマテリアルのバッファアドレスを更新
                    mat_desc.BufferLocation += mat_desc.SizeInBytes;

                    // テクスチャをビューに追加
                    {
                        tex_desc.Format = this->_pmd_textures[i].tex->GetDesc().Format;
                        context->dev->CreateShaderResourceView(
                            this->_pmd_textures[i].tex.Get(),
                            &tex_desc,
                            heap_h
                        );

                        heap_h.ptr += inc_size;
                    }

                    // スフィアテクスチャをビューに追加
                    {
                        sp_tex_multi_desc.Format = this->_pmd_textures[i].sph->GetDesc().Format;
                        context->dev->CreateShaderResourceView(
                            this->_pmd_textures[i].sph.Get(),
                            &sp_tex_multi_desc,
                            heap_h
                        );

                        heap_h.ptr += inc_size;
                    }

                    // 加算スフィアテクスチャをビューに追加
                    {
                        sp_tex_add_desc.Format = this->_pmd_textures[i].spa->GetDesc().Format;
                        context->dev->CreateShaderResourceView(
                            this->_pmd_textures[i].spa.Get(),
                            &sp_tex_add_desc,
                            heap_h
                        );

                        heap_h.ptr += inc_size;
                    }

                    // トゥーンテクスチャをビューに追加
                    {
                        toon_tex_add_desc.Format = this->_pmd_textures[i].toon->GetDesc().Format;
                        context->dev->CreateShaderResourceView(
                            this->_pmd_textures[i].toon.Get(),
                            &toon_tex_add_desc,
                            heap_h
                        );

                        heap_h.ptr += inc_size;
                    }
                }
            }
        }

        const auto& window_data = this->_window_ctrl->GetWindowData();
        {
            {
                // カメラ行列作成
                {
                    DirectX::XMFLOAT3 target(0.0, this->_eye.y, 0.0);
                    DirectX::XMFLOAT3 up(0.0, 1.0f, 0.0);

                    this->_view_mat = DirectX::XMMatrixLookAtLH(
                        DirectX::XMLoadFloat3(&this->_eye),
                        DirectX::XMLoadFloat3(&target),
                        DirectX::XMLoadFloat3(&up));
                }

                // 射影行列作成
                {
                    this->_proj_mat = DirectX::XMMatrixPerspectiveFovLH(
                        DirectX::XM_PIDIV2,
                        static_cast<float>(window_data.width) / static_cast<float>(window_data.height),
                        // カメラが写し始めの値
                        1.0f,
                        // カメラが写し終わりの値
                        100.0f);
                }
            }

            {
                // 定数バッファを作成
                auto buff = DirectX12::CreateBlankResForHeapUpload(context, "pmd_scene_data", (sizeof(SceneShaderData) + 0xff) & ~0xff);
                buff->Map(0, nullptr, (void**)&this->_p_scene_shader_param);
                // マップ解除はしない
                // 更新毎に書き込まれるから
            }
        }

        // ディスクリプタヒープにリソースビューを設定する
        {
            auto basic_heap = context->desc_heap_map["basic_heap"];
            auto basic_heap_handle = basic_heap->GetCPUDescriptorHandleForHeapStart();
            // 行列の定数バッファビューを作る
            {
                D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc = {};

                auto buff = context->res_buff_map["pmd_scene_data"];
                cbv_desc.BufferLocation = buff->GetGPUVirtualAddress();
                LOGD << _T("basic_heap: buff->GetDesc().Width = ") << buff->GetDesc().Width;
                cbv_desc.SizeInBytes = static_cast<UINT>(buff->GetDesc().Width);

                // ディスクリプタヒープのハンドルにビュー作成
                context->dev->CreateConstantBufferView(&cbv_desc, basic_heap_handle);
            }
            // このディスクリプタヒープに対してはリソース一つのみ
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
                D3D12_ROOT_PARAMETER root_params[2] = {};

                // ディスクリプタレンジを作成
                D3D12_DESCRIPTOR_RANGE desc_tbl_ranges[3] = {};
                // 定数のレジスター設定
                {
                    // ディスクリプタヒープ1つ目
                    // 定数用レンスター0番
                    desc_tbl_ranges[0] = CD3DX12_DESCRIPTOR_RANGE(
                        D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
                        1,
                        0
                    );

                    // ディスクリプタヒープ2つ目
                    // マテリアルの定数バッファを定数用レジスター1番に
                    desc_tbl_ranges[1] = CD3DX12_DESCRIPTOR_RANGE(
                        D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
                        1,
                        1
                    );

                    // テクスチャをレジスタ-0番目に
                    desc_tbl_ranges[2] = CD3DX12_DESCRIPTOR_RANGE(
                        D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
                        this->_material_texture_num,
                        0
                    );
                }

                // ルートパラメータを作成
                // ディスクリプタヒープの対応表のディスクリプタテーブルの設定を作成
                {
                    // ディスクリプタヒープの一つ目は[0]のレンジデータ
                    CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(root_params[0], 1, &desc_tbl_ranges[0]);
                    // ディスクリプタヒープの2つ目は[1] / [2]をひとまとめのレンジデータ
                    CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(root_params[1], 2, &desc_tbl_ranges[1]);
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

                auto h_result = DirectX12::CreateRootSignature(context, "pmd_root_sig", &p_error_blob, root_sig_desc);
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
                context,
                "pmd_gpipeline",
                "pmd_root_sig",
                "pmd_vs",
                "pmd_ps",
                input_layout, _countof(input_layout));
        }

        return true;
    }

    void GameController::Update()
    {
    }

    void GameController::Render()
    {
        auto model = this->_window_ctrl->GetModel();
        auto context = model->Context();

        // PMD用のパイプラインステートを設定
        {
            auto pipeline_state = context->_pipeline_state_map["pmd_gpipeline"];
            context->cmd_list->SetPipelineState(pipeline_state.Get());
        }

        // 空バッファに頂点ビュー定義
        D3D12_VERTEX_BUFFER_VIEW vb_view = {};
        // 頂点バッファ構成を出力
        // 毎フレームやっても問題ない？
        // 頂点バッファが変わった時は当然ビューも変わるので更新箇所でやるのがいい気が
        {
            // 内容を決めるバッファを取得
            auto buff = context->res_buff_map["pmd_vs_buff"];
            vb_view.BufferLocation = buff->GetGPUVirtualAddress();
            // バッファ全サイズ
            vb_view.SizeInBytes = this->_pmd_data_pack.vertexs.size() * sizeof(this->_pmd_data_pack.vertexs[0]);
            // バッファ内の一塊となるデータサイズ
            vb_view.StrideInBytes = this->_pmd_data_pack.vertex_size;
        }

        // 空バッファにインデックスビュー定義
        D3D12_INDEX_BUFFER_VIEW ib_view = {};
        {
            // 内容を決めるバッファを取得
            auto buff = context->res_buff_map["pmd_idx_buff"];
            ib_view.BufferLocation = buff->GetGPUVirtualAddress();
            // インデックスの要素データ型を16byteにしている
            ib_view.Format = DXGI_FORMAT_R16_UINT;
            ib_view.SizeInBytes = this->_pmd_data_pack.indices.size() * sizeof(this->_pmd_data_pack.indices[0]);
        }

        // 描画
        context->cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // 一度に設定できるインデックスバッファビューは一つのみ
        context->cmd_list->IASetIndexBuffer(&ib_view);
        // 頂点バッファビューを設定
        context->cmd_list->IASetVertexBuffers(0, 1, &vb_view);

        // シェーダーに受け渡すパラメータ
        {
            this->_local_mat *= DirectX::XMMatrixRotationY(DirectX::XM_PIDIV4 * 0.01);
            this->_p_scene_shader_param->world_mat = this->_local_mat * this->_world_mat;
            this->_p_scene_shader_param->view_mat = this->_view_mat;
            this->_p_scene_shader_param->proj_mat = this->_proj_mat;
            this->_p_scene_shader_param->eye = this->_eye;
        }

        // 利用するディスクリプタと関連付けたルートシグネチャを利用
        auto root_sig = context->_root_sig_map["pmd_root_sig"];
        context->cmd_list->SetGraphicsRootSignature(root_sig.Get());
        {
            // ディスクリプタヒープを設定
            auto p_basic_desc_heap = context->desc_heap_map["basic_heap"];
            assert(p_basic_desc_heap != nullptr);

            // ルートパラメータ0とディスクリプタヒープの関連付け
            context->cmd_list->SetDescriptorHeaps(1, p_basic_desc_heap.GetAddressOf());
            {
                auto basic_heap_handle = p_basic_desc_heap->GetGPUDescriptorHandleForHeapStart();
                context->cmd_list->SetGraphicsRootDescriptorTable(
                    // ルートパラメータインデックス
                    0,
                    // ヒープアドレス
                    basic_heap_handle);
            }

            // マテリアルのディスクリプタヒープ設定
            auto p_material_desc_heap = context->desc_heap_map["pmd_material_heap"];
            context->cmd_list->SetDescriptorHeaps(1, p_material_desc_heap.GetAddressOf());
            {
                // 一つのディスクリプタヒープにある複数ビューを参照するアドレスオフセットサイズ
                auto cbvsrv_inc_size =
                    context->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * this->_material_desc_num;

                auto material_h = p_material_desc_heap->GetGPUDescriptorHandleForHeapStart();
                UINT32 idx_offset = 0;
                for (auto& m : this->_pmd_materials)
                {
                    context->cmd_list->SetGraphicsRootDescriptorTable(
                        // ルートパラメータインデックス
                        1,
                        // ヒープアドレス
                        material_h);

                    context->cmd_list->DrawIndexedInstanced(m.indices_num, 1, idx_offset, 0, 0);

                    material_h.ptr += cbvsrv_inc_size;
                    idx_offset += m.indices_num;
                }
            }
        }
    }

    void GameController::End()
    {
    }
}
