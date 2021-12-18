#pragma once

#include <vector>

// DXをインクルード
// これらのファイル群はどの環境にも入っているわけじゃないよね？
// ue4はエンジンプロジェクトの中にDXのファイル群が含まれていた
#include <d3d12.h>
#include <dxgi1_6.h>

// DirectXTexを利用するため必要
#include <DirectXTex.h>

// DX12の面倒な処理をまとめている
// しかしDirectX12のバージョンが異なるとコンパイルエラーになることもあるので注意
// すくなくともwindows sdkの19041バージョンなら大丈夫だった
#include "d3dx12.h"

// DXのライブラリを含める
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

// DirectXTexを利用するため必要
#pragma comment(lib, "DirectXTex.lib")

namespace Texture
{
	// テクスチャー用のRGBA
	struct TexRGBA
	{
		unsigned char R, G, B, A;
	};

	/// <summary>
	/// 仮のテクスチャーのデータを出力
	/// </summary>
	/// <param name="out_tex_data"></param>
	static void OutputDummyTextureData(std::vector<TexRGBA>& out_tex_data)
	{
		for (auto& rgba : out_tex_data)
		{
			rgba.R = rand() % 256;
			rgba.G = rand() % 256;
			rgba.B = rand() % 256;
			rgba.A = 255;
		}
	}

	/// <summary>
	/// テクスチャーバッファを作成
	/// </summary>
	static ID3D12Resource* CreateBuffer(
		ID3D12Device* in_p_dev,
		UINT64 in_width, UINT in_height,
		// 表示する画像フォーマット
		DXGI_FORMAT in_rgba_format,
		// 奥行きと配列サイズ
		UINT16 in_depth_or_array_size,
		// ミップマップレベル
		UINT16 in_mip_levels,
		// TODO: 分からん！
		D3D12_RESOURCE_DIMENSION in_dimension)
	{
		// 引数の事前チェック
		{
			assert(in_p_dev != nullptr);
			assert(0 < in_width);
			assert(0 < in_height);
		}

		// WriteToSubresourceで転送するためのヒープ設定
		/*
		D3D12_HEAP_PROPERTIES heap_prop = {};
		{
			// 特殊設定なのでカスタム設定にする
			heap_prop.Type = D3D12_HEAP_TYPE_CUSTOM;

			// TODO: ライトバック？
			heap_prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;

			// 転送はL0, という事はCPU側から直接行う
			// TODO: L0とはなに？
			heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

			// 単一アダプターなので0
			// TODO: 単一でない場合はどうなるの？
			heap_prop.CreationNodeMask = 0;
			heap_prop.VisibleNodeMask = 0;
		}
		*/
		// 上記を置き換え
		auto heap_prop = CD3DX12_HEAP_PROPERTIES(
			D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
			D3D12_MEMORY_POOL_L0,
			0, 0);

		/*
		D3D12_RESOURCE_DESC res_desc = {};
		{
			// RGBAフォーマット
			res_desc.Format = in_rgba_format;
			// テクスチャの縦横サイズ
			res_desc.Width = in_width;
			res_desc.Height = in_height;
			// TODO: これの用途が分からん
			res_desc.DepthOrArraySize = in_depth_or_array_size;
			// アンチエイリアシングしないので１に
			res_desc.SampleDesc.Count = 1;
			// クオリティは最低
			res_desc.SampleDesc.Quality = 0;
			// ミップマップしないので１に
			res_desc.MipLevels = in_mip_levels;
			// 2Dテクスチャ用
			res_desc.Dimension = in_dimension;
			// レイアウトは決めない
			res_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			// フラグなし
			res_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		}
		*/
		// 上記を置き換え
		auto res_desc = CD3DX12_RESOURCE_DESC(
			in_dimension,
			0,
			in_width,
			in_height,
			in_depth_or_array_size,
			in_mip_levels,
			in_rgba_format,
			1,
			0,
			D3D12_TEXTURE_LAYOUT_UNKNOWN,
			D3D12_RESOURCE_FLAG_NONE);

		ID3D12Resource* p_resouce = nullptr;
		auto result = in_p_dev->CreateCommittedResource(
			&heap_prop,
			D3D12_HEAP_FLAG_NONE,
			&res_desc,
			// テクスチャ用
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&p_resouce));
		assert(SUCCEEDED(result));

		return p_resouce;
	}

	/// <summary>
	/// テクスチャが参照できるヒープ構成にする
	/// </summary>
	/// <param name="in_p_dev"></param>
	/// <param name="in_p_tex_buff"></param>
	/// <param name="in_p_tex_desc_heap"></param>
	/// <param name="in_rgba_format"></param>
	/// <returns></returns>
	static bool CreateShaderResourceView(
		ID3D12Device* in_p_dev,
		ID3D12Resource* in_p_tex_buff,
		DXGI_FORMAT in_rgba_format,
		D3D12_CPU_DESCRIPTOR_HANDLE& in_r_desc_handle)
	{
		assert(in_p_dev != nullptr);
		assert(in_p_tex_buff != nullptr);

		D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};

		// UNORM => unsigne normalize
		// つまりRGBAの値0 - 255 を正規化した0 - 1として扱いますという指定
		srv_desc.Format = in_rgba_format;

		// 指定したフォーマットでデータ通りの順序で割り当てられている事とする
		srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		// 上記の設定から以下のメンバー変数の利用を決めている
		{
			srv_desc.Texture2D.MipLevels = 1;
		}

		// ヒープ構成を設定したリソースビューに変える
		in_p_dev->CreateShaderResourceView(
			// テクスチャーデータを持っているバッファを指定
			in_p_tex_buff,
			// リソースビューの設定データ
			&srv_desc,
			// リソースビューを割り当てるヒープを指定
		// ----------------------------------------------------------
			in_r_desc_handle);
		// ----------------------------------------------------------

		return true;
	}

	/// <summary>
	/// 画像ファイルをロード
	/// </summary>
	/// <param name="out_p_metadata"></param>
	/// <param name="out_p_scratch_img"></param>
	/// <param name="in_p_load_filepath"></param>
	/// <returns></returns>
	static bool LoadImageFile(
		DirectX::TexMetadata* out_p_metadata,
		DirectX::ScratchImage* out_p_scratch_img,
		const WCHAR* in_p_load_filepath)
	{
		{
			assert(in_p_load_filepath != nullptr);
			assert(out_p_metadata != nullptr);
			assert(out_p_scratch_img != nullptr);
		}

		auto result = DirectX::LoadFromWICFile(in_p_load_filepath, DirectX::WIC_FLAGS_NONE, out_p_metadata, *out_p_scratch_img);
		assert(SUCCEEDED(result));

		return true;
	}

	/// <summary>
	/// 画像を転送するためのアップロード用の中間ヒープリソースを作る
	/// </summary>
	/// <param name="in_p_dev"></param>
	/// <param name="in_width"></param>
	/// <returns></returns>
	static ID3D12Resource* CreateUploadResoucre(
		ID3D12Device* in_p_dev,
		UINT64 in_width)
	{
		// 中間バッファとなるアップロードヒープ設定
		/*
		D3D12_HEAP_PROPERTIES upload_heapprop = {};
		{
			// マップ可能にするためにUPLOAD設定
			upload_heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;

			// アップロード用なのでCPU/メモリは利用しない
			upload_heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			upload_heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

			// 単一アダプタのため0で良い
			upload_heapprop.CreationNodeMask = 0;
			upload_heapprop.VisibleNodeMask = 0;
		}
		*/
		// 上記を置き換え
		auto upload_heapprop = CD3DX12_HEAP_PROPERTIES(
			D3D12_HEAP_TYPE_UPLOAD,
			0, 0);

		// アップロード用のリソース設定
		/*
		D3D12_RESOURCE_DESC res_desc = {};
		{
			// データの塊となるので未指定でいい
			res_desc.Format = DXGI_FORMAT_UNKNOWN;
			// 単なるバッファ指定
			res_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;

			// データサイズ
			res_desc.Width = in_width;
			res_desc.Height = 1;
			res_desc.DepthOrArraySize = 1;
			res_desc.MipLevels = 1;

			// 画像なので連続したデータなので連続データとして扱う
			res_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			res_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

			res_desc.SampleDesc.Count = 1;
			res_desc.SampleDesc.Quality = 0;
		}
		*/
		// 上記を置き換えている
		auto res_desc = CD3DX12_RESOURCE_DESC::Buffer(in_width);

		// 中間バッファを作成
		ID3D12Resource* p_upload_buffer = nullptr;
		{
			auto result = in_p_dev->CreateCommittedResource(
				&upload_heapprop,
				D3D12_HEAP_FLAG_NONE,
				&res_desc,
				// CPUからデータを転送する事はできる
				// しかしGPUkからは読み込みしか出来ない指定
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&p_upload_buffer)

			);
			assert(SUCCEEDED(result));
		}

		return p_upload_buffer;
	}

	/// <summary>
	/// テクスチャーコピーするためのリソースを作成
	/// </summary>
	/// <param name="in_p_dev"></param>
	/// <param name="in_metadata"></param>
	/// <returns></returns>
	static ID3D12Resource* CreateCopyTextureResoucre(
		ID3D12Device* in_p_dev,
		const DirectX::TexMetadata& in_metadata)
	{
		// ヒープ設定
		/*
		D3D12_HEAP_PROPERTIES texture_heapprop = {};
		{
			// テクスチャ用
			texture_heapprop.Type = D3D12_HEAP_TYPE_DEFAULT;

			// CPU/メモリは利用しない
			texture_heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			texture_heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

			// 単一アダプタのため0で良い
			texture_heapprop.CreationNodeMask = 0;
			texture_heapprop.VisibleNodeMask = 0;
		}
		*/
		auto texture_heapprop = CD3DX12_HEAP_PROPERTIES(
			D3D12_HEAP_TYPE_DEFAULT, 0, 0);

		// リソース設定
		/*
		D3D12_RESOURCE_DESC res_desc = {};
		{
			// データの塊となるので未指定でいい
			res_desc.Format = in_metadata.format;

			res_desc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(in_metadata.dimension);

			// データサイズ
			res_desc.Width = in_metadata.width;;
			res_desc.Height = in_metadata.height;
			res_desc.DepthOrArraySize = static_cast<UINT16>(in_metadata.arraySize);
			res_desc.MipLevels = static_cast<UINT16>(in_metadata.mipLevels);

			res_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

			res_desc.SampleDesc.Count = 1;
			res_desc.SampleDesc.Quality = 0;
		}
		*/
		// 上記を置き換えている
		auto res_desc = CD3DX12_RESOURCE_DESC(
			static_cast<D3D12_RESOURCE_DIMENSION>(in_metadata.dimension),
			0,
			in_metadata.width,
			in_metadata.height,
			static_cast<UINT16>(in_metadata.arraySize),
			static_cast<UINT16>(in_metadata.mipLevels),
			in_metadata.format,
			1,
			0,
			D3D12_TEXTURE_LAYOUT_UNKNOWN,
			D3D12_RESOURCE_FLAG_NONE);

		// バッファを作成
		ID3D12Resource* p_buffer = nullptr;
		{
			auto result = in_p_dev->CreateCommittedResource(
				&texture_heapprop,
				D3D12_HEAP_FLAG_NONE,
				&res_desc,
				// コピー先とする
				D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr,
				IID_PPV_ARGS(&p_buffer)

			);
			assert(SUCCEEDED(result));
		}

		return p_buffer;
	}
}
