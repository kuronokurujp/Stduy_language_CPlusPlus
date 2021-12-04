#pragma once

//  シェーダーをテスト動作するためのコード
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib");

namespace DirectX
{
	ID3DBlob* s_p_vs_blob = nullptr;
	ID3DBlob* s_p_ps_blob = nullptr;

	static void ErrorShader(std::string* out_p_error, HRESULT in_result, ID3DBlob* in_p_error_blob)
	{
		if (!FAILED(in_result))
			return;

		if (in_result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			if (out_p_error != nullptr)
			{
				out_p_error->append("shader file find not");
			}

			return;
		}

		if (out_p_error != nullptr && in_p_error_blob != nullptr)
		{
			out_p_error->resize(in_p_error_blob->GetBufferSize());
			::std::copy_n(
				(char*)in_p_error_blob->GetBufferPointer(),
				in_p_error_blob->GetBufferSize(),
				out_p_error->begin());
		}
	}

	static bool LoadVSShaderFileSample(std::string* out_p_error)
	{
		ID3DBlob* p_error_blob = nullptr;
		auto result = D3DCompileFromFile(
			L"BasicVertexShader.hlsl",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"BasicVS", "vs_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&s_p_vs_blob, &p_error_blob);

		if (SUCCEEDED(result))
			return true;

		ErrorShader(out_p_error, result, p_error_blob);

		return false;
	}

	static bool LoadPSShaderFileSample(std::string* out_p_error)
	{
		ID3DBlob* p_error_blob = nullptr;
		auto result = D3DCompileFromFile(
			L"BasicPixelShader.hlsl",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"BasicPS", "ps_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&s_p_ps_blob, &p_error_blob);

		if (SUCCEEDED(result))
			return true;

		ErrorShader(out_p_error, result, p_error_blob);

		return false;
	}
}
