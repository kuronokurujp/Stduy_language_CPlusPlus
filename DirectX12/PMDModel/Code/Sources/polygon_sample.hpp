#pragma

#include <algorithm>
#include <iostream>
#include <iterator>

// DirectXが用意している数学定義
#include <DirectXMath.h>

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

namespace DirectX
{
	/// <summary>
	/// コミットリソースを作成.
	/// </summary>
	/// <returns></returns>
	static ID3D12Resource* CreateResource(ID3D12Device* in_p_dev, UINT64 in_res_size)
	{
		assert(in_p_dev != nullptr);

		auto heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD, 0, 0);
		auto res_desc = CD3DX12_RESOURCE_DESC::Buffer(in_res_size);

		// 設定内容に基づいて頂点バッファを作成
		ID3D12Resource* p_buff = nullptr;
		auto result = in_p_dev->CreateCommittedResource(
			&heap_prop,
			D3D12_HEAP_FLAG_NONE,
			&res_desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&p_buff)
		);
		assert(SUCCEEDED(result));

		return p_buff;
	}

	/// <summary>
	/// Copies the vertices data.
	/// </summary>
	/// <param name="in_p_vertcies_res">The in p vertcies resource.</param>
	/// <param name="in_p_vertices_first">The in p vertices first.</param>
	/// <param name="in_p_vertices_end">The in p vertices end.</param>
	template <class T, class S>
	static void CopyBuffer(ID3D12Resource* in_p_vertcies_res, T in_p_vertices_first, T in_p_vertices_end)
	{
		assert(in_p_vertcies_res != nullptr);

		S* p_vert_map = nullptr;
		// バッファを書き込みするために書き込み先のメモリをロック
		auto result = in_p_vertcies_res->Map(0, nullptr, (void**)&p_vert_map);
		assert(result == S_OK);

		// バッファを書き込む
		std::copy(in_p_vertices_first, in_p_vertices_end, p_vert_map);

		// ロックしたメモリを解放
		in_p_vertcies_res->Unmap(0, nullptr);
	}

	static void OutputVerticesBufferView(
		D3D12_VERTEX_BUFFER_VIEW* out_p_buffer_view,
		ID3D12Resource* const in_p_resource,
		const UINT in_vertices_all_size,
		const UINT in_vertices_size)
	{
		assert(out_p_buffer_view != nullptr);

		out_p_buffer_view->BufferLocation = in_p_resource->GetGPUVirtualAddress();
		// 頂点バッファのデータ全サイズ
		out_p_buffer_view->SizeInBytes = in_vertices_all_size;
		// 頂点バッファの頂点一つ辺りのデータサイズ, このサイズで区切る
		out_p_buffer_view->StrideInBytes = in_vertices_size;
	}
}
