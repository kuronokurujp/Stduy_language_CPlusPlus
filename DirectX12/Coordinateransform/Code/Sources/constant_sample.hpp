#pragma once

#include <assert.h>

// DXをインクルード
// これらのファイル群はどの環境にも入っているわけじゃないよね？
// ue4はエンジンプロジェクトの中にDXのファイル群が含まれていた
#include <d3d12.h>
#include <dxgi1_6.h>

// DX12の面倒な処理をまとめている
// しかしDirectX12のバージョンが異なるとコンパイルエラーになることもあるので注意
// すくなくともwindows sdkの19041バージョンなら大丈夫だった
#include "d3dx12.h"

// DXのライブラリを含める
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

namespace Constant
{
	/// <summary>
	/// 定数バッファを作成
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="in_p_dev"></param>
	/// <param name="in_data"></param>
	/// <returns></returns>
	template <class T>
	static ID3D12Resource* CreateBuffer(ID3D12Device* in_p_dev, T& in_data)
	{
		assert(in_p_dev != nullptr);

		ID3D12Resource* p_const_buff = nullptr;

		auto heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto res_desc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(in_data) + 0xff) & ~0xff);
		auto result = in_p_dev->CreateCommittedResource(
			&heap_prop,
			D3D12_HEAP_FLAG_NONE,
			// 256のアライメントにしないとだめ
			&res_desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&p_const_buff)
		);
		assert(SUCCEEDED(result));

		return p_const_buff;
	}

	/// <summary>
	/// 定数バッファのバッファビューを作成
	/// </summary>
	/// <param name="in_p_dev"></param>
	/// <param name="in_p_buff"></param>
	/// <param name="in_r_desc_handle"></param>
	/// <returns></returns>
	static bool CreateBufferView(
		ID3D12Device* in_p_dev,
		ID3D12Resource* in_p_buff,
		D3D12_CPU_DESCRIPTOR_HANDLE& in_r_desc_handle)
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc = {};
		cbv_desc.BufferLocation = in_p_buff->GetGPUVirtualAddress();
		cbv_desc.SizeInBytes = in_p_buff->GetDesc().Width;

		// ディスクリプタヒープのハンドルにビュー作成
		in_p_dev->CreateConstantBufferView(&cbv_desc, in_r_desc_handle);
		return true;
	}
}
