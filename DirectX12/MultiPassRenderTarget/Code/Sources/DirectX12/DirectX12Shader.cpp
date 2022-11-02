#include "DirectX12/DirectX12Shader.h"

#include <assert.h>

#pragma comment(lib, "d3dcompiler.lib")

namespace DirectX12
{
    namespace Impl
    {
        static const HRESULT LoadShaderBySync(
            ComPtr<ID3D10Blob>& out_p_blob,
            ID3D10Blob** out_pp_error_blob,
            const WCHAR* in_shader_filepath,
            const std::string& in_r_entrypoint_name,
            const std::string& in_r_shader_target_name)
        {
            assert(out_pp_error_blob != nullptr);

            auto result = D3DCompileFromFile(
                in_shader_filepath,
                nullptr,
                D3D_COMPILE_STANDARD_FILE_INCLUDE,
                in_r_entrypoint_name.c_str(),
                in_r_shader_target_name.c_str(),
                D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
                0,
                out_p_blob.ReleaseAndGetAddressOf(), out_pp_error_blob);

            if (FAILED(result))
                return result;

            return result;
        }
    }

    /// <summary>
    /// 頂点シェーダーファイルの同期ロード
    /// </summary>
    /// <param name="in_p_context"></param>
    /// <param name="in_r_key_name"></param>
    /// <param name="out_pp_error_blob"></param>
    /// <param name="in_shader_filepath"></param>
    /// <returns></returns>
    const HRESULT LoadShaderVSBySync(std::shared_ptr<Context> in_p_context, const std::string& in_r_key_name, ID3D10Blob** out_pp_error_blob, const WCHAR* in_shader_filepath)
    {
        assert(in_p_context != nullptr);
        assert(out_pp_error_blob != nullptr);
        assert(!in_r_key_name.empty());

        ComPtr<ID3D10Blob> vs_blob;

        auto result = Impl::LoadShaderBySync(
            vs_blob,
            out_pp_error_blob,
            in_shader_filepath,
            "BasicVS", "vs_5_0");

        if (FAILED(result))
            return result;

        in_p_context->p_vs_blob_map[in_r_key_name.c_str()] = vs_blob;

        return result;
    }

    /// <summary>
    /// ピクセルシェーダーファイルの同期ロード
    /// </summary>
    /// <param name="in_p_context"></param>
    /// <param name="in_r_key_name"></param>
    /// <param name="out_pp_error_blob"></param>
    /// <param name="in_shader_filepath"></param>
    /// <returns></returns>
    const HRESULT LoadShaderPSBySync(std::shared_ptr<Context> in_p_context, const std::string& in_r_key_name, ID3D10Blob** out_pp_error_blob, const WCHAR* in_shader_filepath)
    {
        assert(in_p_context != nullptr);
        assert(out_pp_error_blob != nullptr);
        assert(!in_r_key_name.empty());

        ComPtr<ID3D10Blob> ps_blob;
        auto result = Impl::LoadShaderBySync(
            ps_blob,
            out_pp_error_blob,
            in_shader_filepath,
            "BasicPS", "ps_5_0");

        if (FAILED(result))
            return result;

        in_p_context->p_ps_blob_map[in_r_key_name.c_str()] = ps_blob;

        return result;
    }
}
