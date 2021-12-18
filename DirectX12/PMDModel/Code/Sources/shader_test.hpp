#pragma once

//  シェーダーをテスト動作するためのコード
#include <d3dcompiler.h>
#include "common.hpp"

#pragma comment(lib, "d3dcompiler.lib")

namespace DirectX
{
	static bool LoadVSShaderFileSample(
		ID3D10Blob** out_pp_vs_blob,
		std::string* out_p_error,
		const WCHAR* in_shader_filepath)
	{
		assert(out_pp_vs_blob != nullptr);

		ID3DBlob* p_error_blob = nullptr;
		auto result = D3DCompileFromFile(
			in_shader_filepath,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"BasicVS", "vs_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			out_pp_vs_blob, &p_error_blob);

		if (SUCCEEDED(result))
			return true;

		Common::OutputErrorBlob(out_p_error, result, p_error_blob);

		return false;
	}

	static bool LoadPSShaderFileSample(
		ID3D10Blob** out_pp_ps_blob,
		std::string* out_p_error,
		const WCHAR* in_shader_filepath)
	{
		assert(out_pp_ps_blob != nullptr);

		ID3DBlob* p_error_blob = nullptr;
		auto result = D3DCompileFromFile(
			in_shader_filepath,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"BasicPS", "ps_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			out_pp_ps_blob, &p_error_blob);

		if (SUCCEEDED(result))
			return true;

		Common::OutputErrorBlob(out_p_error, result, p_error_blob);

		return false;
	}
}
