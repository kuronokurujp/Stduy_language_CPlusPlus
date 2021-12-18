#pragma once

namespace Common
{
	/// <summary>
	/// 設定した値をアライメント幅に合わせて返している
	/// </summary>
	/// <param name="in_size"></param>
	/// <param name="in_alignment"></param>
	/// <returns></returns>
	static size_t AlignmentedSize(size_t in_size, size_t in_alignment)
	{
		// in_alignment倍数として返す
		return in_size + in_alignment - (in_size % in_alignment);
	}

	/// <summary>
	/// ディスクリプタヒープ作成
	/// </summary>
	/// <param name="in_p_dev"></param>
	/// <returns></returns>
	static ID3D12DescriptorHeap* CreateDescriptorHeap(
		ID3D12Device* in_p_dev,
		UINT in_descriptors_num)
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc_heap_desc = {};
		// TODO: シェーダーから見えるようにする？
		// どゆこと
		desc_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		desc_heap_desc.NodeMask = 0;

		// 作成するディスクリプタの数
		desc_heap_desc.NumDescriptors = in_descriptors_num;

		// シェーダーリソースビュー用
		desc_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

		ID3D12DescriptorHeap* p_tex_desc_heap = nullptr;
		auto result = in_p_dev->CreateDescriptorHeap(&desc_heap_desc, IID_PPV_ARGS(&p_tex_desc_heap));
		assert(SUCCEEDED(result));

		return p_tex_desc_heap;
	}
}
