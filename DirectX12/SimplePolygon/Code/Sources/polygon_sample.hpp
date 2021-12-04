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

// DXのライブラリを含める
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

namespace DirectX
{
	/// <summary>
	/// 頂点バッファを生成.
	/// </summary>
	/// <returns></returns>
	static ID3D12Resource* CreateVertices(ID3D12Device* in_p_dev, XMFLOAT3 in_vertices[])
	{
		assert(in_p_dev != nullptr);

		D3D12_HEAP_PROPERTIES heap_prop = {};

		heap_prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		heap_prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC res_desc = {};

		res_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		res_desc.Width = sizeof(in_vertices);
		res_desc.Height = 1;
		res_desc.DepthOrArraySize = 1;
		res_desc.MipLevels = 1;
		res_desc.Format = DXGI_FORMAT_UNKNOWN;
		res_desc.SampleDesc.Count = 1;
		res_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		res_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		ID3D12Resource* p_vert_buff = nullptr;
		auto result = in_p_dev->CreateCommittedResource(
			&heap_prop,
			D3D12_HEAP_FLAG_NONE,
			&res_desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&p_vert_buff)
		);
		assert(result == S_OK);

		return p_vert_buff;
	}

	/// <summary>
	/// Copies the vertices data.
	/// </summary>
	/// <param name="in_p_vertcies_res">The in p vertcies resource.</param>
	/// <param name="in_p_vertices_first">The in p vertices first.</param>
	/// <param name="in_p_vertices_end">The in p vertices end.</param>
	static void CopyVerticesData(ID3D12Resource* in_p_vertcies_res, XMFLOAT3* in_p_vertices_first, XMFLOAT3* in_p_vertices_end)
	{
		assert(in_p_vertcies_res != nullptr);
		assert(in_p_vertices_first != nullptr);
		assert(in_p_vertices_end != nullptr);

		XMFLOAT3* p_vert_map = nullptr;
		// バッファを書き込みするために書き込み先のメモリをロック
		auto result = in_p_vertcies_res->Map(0, nullptr, (void**)&p_vert_map);
		assert(result == S_OK);

		// バッファを書き込む
		std::copy(in_p_vertices_first, in_p_vertices_end, p_vert_map);

		// ロックしたメモリを解放
		in_p_vertcies_res->Unmap(0, nullptr);
	}

	static void OutputVerticesBufferView(
		D3D12_VERTEX_BUFFER_VIEW* out_p_buffer_view, ID3D12Resource* const in_p_resource,
		const UINT in_vertices_all_size, const UINT in_vertices_size)
	{
		assert(out_p_buffer_view != nullptr);

		out_p_buffer_view->BufferLocation = in_p_resource->GetGPUVirtualAddress();
		// 頂点バッファのデータ全サイズ
		out_p_buffer_view->SizeInBytes = in_vertices_all_size;
		// 頂点バッファの頂点一つ辺りのデータサイズ, このサイズで区切る
		out_p_buffer_view->StrideInBytes = in_vertices_size;
	}
}
