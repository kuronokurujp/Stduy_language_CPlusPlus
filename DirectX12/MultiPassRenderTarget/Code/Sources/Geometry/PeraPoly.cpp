#include "Geometry/PeraPoly.h"

#include "Common.h"

#include "DirectX12/DirectX12MiniHeader.h"
#include "DirectX12/DirectX12Mesh.h"

namespace Geometry
{
    PeraPoly::Vertex PeraPoly::s_pv[4] = {
        // 左下
        {{-1.0f, -1.0f, 0.1f}, {0.0f, 1.0f}},
        // 左上
        {{-1.0f, 1.0f, 0.1f}, {0.0f, 0.0f}},
        // 右下
        {{1.0f, -1.0f, 0.1f}, {1.0f, 1.0f}},
        // 右上
        {{1.0f, 1.0f, 0.1f}, {1.0f, 0.0f}},
    };

    PeraPoly::PeraPoly(
        const std::string_view& in_r_shader_vs_filepath,
        const std::string_view& in_r_shader_ps_filepath)
    {
        this->_ps_filepath = in_r_shader_ps_filepath;
        this->_vs_filepath = in_r_shader_vs_filepath;

        this->_mesh = std::make_shared<DirectX12::Mesh>("pera_vs_buff" + Common::CreateGUIDString(), "pera_idx_buff" + Common::CreateGUIDString());
        this->_gps_key = "pera_gps_" + Common::CreateGUIDString();
        this->_sig_key = "pera_sig_" + Common::CreateGUIDString();
    }

    bool PeraPoly::Create(std::shared_ptr<DirectX12::Context> in_context)
    {
        std::vector<PeraPoly::Vertex> vertexs(_countof(PeraPoly::s_pv));
        for (int i = 0; i < _countof(PeraPoly::s_pv); ++i)
            vertexs[i] = PeraPoly::s_pv[i];

        auto s = sizeof(s_pv);
        auto ret = this->_mesh->Create<PeraPoly::Vertex>(in_context, vertexs);

        // TODO: エラー対応を追加

        // 頂点とピクセルシェーダーをロード
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

        // TODO: パイプラインとルートシグネチャを作成
        {
            // 頂点シェーダーのための準備
            D3D12_GRAPHICS_PIPELINE_STATE_DESC gps_desc = {};
            {
                // シェーダーのパラメータを定義
                D3D12_INPUT_ELEMENT_DESC layouts[] = {
                    {
                        "POSITION",
                        0,
                        DXGI_FORMAT_R32G32B32_FLOAT,
                        0,
                        D3D12_APPEND_ALIGNED_ELEMENT,
                        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                        0
                    },
                    {
                        "TEXCOORD",
                        0,
                        DXGI_FORMAT_R32G32_FLOAT,
                        0,
                        D3D12_APPEND_ALIGNED_ELEMENT,
                        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                        0
                    }
                };

                D3D12_GRAPHICS_PIPELINE_STATE_DESC gps_desc = {};
                {
                    gps_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
                    gps_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
                    gps_desc.NumRenderTargets = 1;
                    gps_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
                    gps_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
                    gps_desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
                    gps_desc.SampleDesc.Count = 1;
                    gps_desc.SampleDesc.Quality = 0;
                    gps_desc.DepthStencilState.DepthEnable = false;
                    gps_desc.DepthStencilState.StencilEnable = false;
                    gps_desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
                }

                CD3DX12_ROOT_SIGNATURE_DESC rs_desc = {};
                {
                    D3D12_STATIC_SAMPLER_DESC sampler = CD3DX12_STATIC_SAMPLER_DESC(0);
                    rs_desc.pStaticSamplers = &sampler;
                    rs_desc.NumParameters = 0;
                    rs_desc.NumStaticSamplers = 0;
                    rs_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

                    DirectX12::ComPtr<ID3DBlob> error;
                    auto h_result = DirectX12::CreateRootSignature(in_context, this->_sig_key, error.GetAddressOf(), rs_desc);
                    // TODO: エラー対応は後
                }

                auto h_result = DirectX12::CreateCustomGraphicsPipeline(
                    in_context,
                    this->_gps_key,
                    gps_desc,
                    this->_sig_key,
                    this->_vs_filepath,
                    this->_ps_filepath,
                    layouts,
                    _countof(layouts)
                );
                // TODO: エラー対応を追加
            }
        }

        return ret;
    }

    void PeraPoly::Render(std::shared_ptr<DirectX12::Context> in_context)
    {
        // TODO: パイプラインとルートシグネチャをコマンドに投げる
        in_context->cmd_list->SetGraphicsRootSignature(in_context->_root_sig_map[this->_sig_key.c_str()].Get());
        in_context->cmd_list->SetPipelineState(in_context->_pipeline_state_map[this->_gps_key.c_str()].Get());
        this->_mesh->InsertCmdToCmdPipeline(in_context, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        // TODO: マテリアルの描画
        in_context->cmd_list->DrawInstanced(4, 1, 0, 0);
    }
}
