#pragma once

#include <Windows.h>
#include <string>

// DirectXが用意している数学定義
#include <DirectXMath.h>

#include "texture_sample.hpp"

namespace Material
{
	// シェーダー側に与えるマテリアルデータ
	struct MaterialForHlsl
	{
		DirectX::XMFLOAT3 diffuse;
		float alpha;

		DirectX::XMFLOAT3 speclar;
		float specularity;

		DirectX::XMFLOAT3 ambient;
	};

	// オプションマテリアルデータ
	struct AdditionalMaterial
	{
		std::string tex_path;
		UINT32 toon_idx;
		bool edge_flag;
	};

	// マテリアルデータ
	struct Material
	{
		UINT32 indices_num;
		MaterialForHlsl basic;
		AdditionalMaterial additional;
	};

	/// <summary>
	/// マテリアルのディスクリプタヒープ作成
	/// </summary>
	/// <param name="in_p_dev"></param>
	/// <param name="in_desc_num"></param>
	/// <returns></returns>
	static ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* in_p_dev, const UINT32 in_desc_num)
	{
		ID3D12DescriptorHeap* p_material_heap = nullptr;

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		desc.NodeMask = 0;
		desc.NumDescriptors = in_desc_num;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

		auto result = in_p_dev->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&p_material_heap));
		assert(SUCCEEDED(result));

		return p_material_heap;
	}

	/// <summary>
	/// マテリアルバッファビューを作成
	/// </summary>
	static void CreateBufferView(
		ID3D12Device* in_p_dev,
		ID3D12Resource* in_buff,
		ID3D12DescriptorHeap* in_p_heap,
		std::vector<Material>& in_r_materials,
		std::vector<Texture::Resource>& in_r_texture_resoucres,
		std::vector<Texture::Resource>& in_r_sph_resources,
		std::vector<Texture::Resource>& in_r_spa_resources,
		std::vector<Texture::Resource>& in_r_toon_resources,
		size_t in_alignmented_size)
	{
		// マテリアルの数と設定するテクスチャの数は同じにしないとだめ
		assert(in_r_materials.size() == in_r_texture_resoucres.size());

		// マテリアル定数バッファビューの設定データ
		D3D12_CONSTANT_BUFFER_VIEW_DESC mat_desc = {};
		{
			mat_desc.BufferLocation = in_buff->GetGPUVirtualAddress();
			mat_desc.SizeInBytes = in_alignmented_size;
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

		auto heap_h = in_p_heap->GetCPUDescriptorHandleForHeapStart();
		auto inc_size = in_p_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// マテリアルデータ数分ビューを作成
		for (size_t i = 0; i < in_r_materials.size(); ++i)
		{
			// マテリアルの定数バッファビューを作成
			in_p_dev->CreateConstantBufferView(&mat_desc, heap_h);
			heap_h.ptr += inc_size;

			// 設定するマテリアルのバッファアドレスを更新
			mat_desc.BufferLocation += in_alignmented_size;

			// テクスチャをビューに追加
			{
				tex_desc.Format = in_r_texture_resoucres[i].p_buf->GetDesc().Format;

				in_p_dev->CreateShaderResourceView(
					in_r_texture_resoucres[i].p_buf,
					&tex_desc,
					heap_h
				);

				heap_h.ptr += inc_size;
			}

			// スフィアテクスチャをビューに追加
			{
				sp_tex_multi_desc.Format = in_r_sph_resources[i].p_buf->GetDesc().Format;
				in_p_dev->CreateShaderResourceView(
					in_r_sph_resources[i].p_buf,
					&sp_tex_multi_desc,
					heap_h
				);

				heap_h.ptr += inc_size;
			}

			// 加算スフィアテクスチャをビューに追加
			{
				sp_tex_add_desc.Format = in_r_spa_resources[i].p_buf->GetDesc().Format;
				in_p_dev->CreateShaderResourceView(
					in_r_spa_resources[i].p_buf,
					&sp_tex_add_desc,
					heap_h
				);

				heap_h.ptr += inc_size;
			}

			// トゥーンテクスチャをビューに追加
			{
				toon_tex_add_desc.Format = in_r_toon_resources[i].p_buf->GetDesc().Format;
				in_p_dev->CreateShaderResourceView(
					in_r_toon_resources[i].p_buf,
					&toon_tex_add_desc,
					heap_h
				);

				heap_h.ptr += inc_size;
			}
		}
	}
}
