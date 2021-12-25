#pragma once

#include "DirectX12Core.h"

// シェーダーを扱うのに必要
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include <assert.h>

namespace DirectX12
{
	// DirectX12のシェーダー制御

	/// <summary>
	/// 頂点シェーダーファイルの同期ロード
	/// </summary>
	/// <param name="in_p_context"></param>
	/// <param name="in_r_key_name"></param>
	/// <param name="out_pp_error_blob"></param>
	/// <param name="in_shader_filepath"></param>
	/// <returns></returns>
	extern const HRESULT SyncLoadShaderVS(std::shared_ptr<Context> in_p_context, const std::string& in_r_key_name, ID3D10Blob** out_pp_error_blob, const WCHAR* in_shader_filepath);

	/// <summary>
	/// ピクセルシェーダーファイルの同期ロード
	/// </summary>
	/// <param name="in_p_context"></param>
	/// <param name="in_r_key_name"></param>
	/// <param name="out_pp_error_blob"></param>
	/// <param name="in_shader_filepath"></param>
	/// <returns></returns>
	extern const HRESULT SyncLoadShaderPS(std::shared_ptr<Context> in_p_context, const std::string& in_r_key_name, ID3D10Blob** out_pp_error_blob, const WCHAR* in_shader_filepath);
}
