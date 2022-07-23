#pragma once

#include <vector>
#include <map>

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

#include "common.hpp"

// DXのライブラリを含める
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

// DirectXTexを利用するため必要
#pragma comment(lib, "DirectXTex.lib")

namespace Texture
{
	struct Resource
	{
		ID3D12Resource* p_buf = nullptr;
	};

	// テクスチャー用のRGBA
	struct TexRGBA
	{
		unsigned char R, G, B, A;
	};

	// ロードしたテクスチャリソースをキャッシュ用
	// TODO: このままでは外部公開しまうが一旦これで
	static std::map<std::string, ID3D12Resource*> _s_cache_texture_resoucre;

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
		auto heap_prop = CD3DX12_HEAP_PROPERTIES(
			D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
			D3D12_MEMORY_POOL_L0,
			0, 0);

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
			in_r_desc_handle);

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
		const std::string& in_r_load_filepath)
	{
		{
			assert(out_p_metadata != nullptr);
			assert(out_p_scratch_img != nullptr);
		}

		static std::map<std::string, std::function<HRESULT(const std::wstring& path, DirectX::TexMetadata*, DirectX::ScratchImage*)>> load_lambda_table;
		if (load_lambda_table.empty())
		{
			// WICFileでロード可能なファイル拡張子の登録
			{
				load_lambda_table["spa"] =
					[](
						const std::wstring& in_path,
						DirectX::TexMetadata* out_p_metadata,
						DirectX::ScratchImage* out_p_scratch_img) -> HRESULT
				{
					return DirectX::LoadFromWICFile(in_path.c_str(), DirectX::WIC_FLAGS_NONE, out_p_metadata, *out_p_scratch_img);
				};

				load_lambda_table["bmp"] = load_lambda_table["spa"];
				load_lambda_table["sph"] = load_lambda_table["spa"];
				load_lambda_table["jpg"] = load_lambda_table["spa"];
			}

			// TGAファイルロード登録
			{
				load_lambda_table["tga"] =
					[](
						const std::wstring& in_path,
						DirectX::TexMetadata* out_p_metadata,
						DirectX::ScratchImage* out_p_scratch_img) -> HRESULT
				{
					return DirectX::LoadFromTGAFile(in_path.c_str(), out_p_metadata, *out_p_scratch_img);
				};
			}

			// ddsファイルロード登録
			{
				load_lambda_table["dds"] =
					[](
						const std::wstring& in_path,
						DirectX::TexMetadata* out_p_metadata,
						DirectX::ScratchImage* out_p_scratch_img) -> HRESULT
				{
					return DirectX::LoadFromDDSFile(in_path.c_str(), DirectX::DDS_FLAGS_NONE, out_p_metadata, *out_p_scratch_img);
				};
			}
		}

		auto extension = Common::GetFileExtension(in_r_load_filepath);
		// keyの型をstringにしているのでstring型の変数を引数に与えればいいかと思ったが、
		// 登録したkeyのstring型変数をそのまま渡すと失敗する！
		// string::c_str()を使うと正しいfindは成功する。
		// なぜ？
		// mapのkeyをリテラル文字列すると発生するようだ
		if (load_lambda_table.find(extension.c_str()) == load_lambda_table.end())
			return false;

		auto load_file_path_wstring = Common::ConvertStringToWideString(in_r_load_filepath);
		auto result = load_lambda_table[extension.c_str()](load_file_path_wstring, out_p_metadata, out_p_scratch_img);
		return SUCCEEDED(result);
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
		auto upload_heapprop = CD3DX12_HEAP_PROPERTIES(
			D3D12_HEAP_TYPE_UPLOAD,
			0, 0);

		// アップロード用のリソース設定
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
		auto texture_heapprop = CD3DX12_HEAP_PROPERTIES(
			D3D12_HEAP_TYPE_DEFAULT, 0, 0);

		// リソース設定
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

	/// <summary>
	/// 単色テクスチャリソース作成
	/// </summary>
	/// <param name="out_p_texture"></param>
	/// <param name="in_p_dev"></param>
	/// <returns></returns>
	static bool CreateTextureResourceFromColor(Resource* out_p_texture, ID3D12Device* in_p_dev, UINT8 in_color)
	{
		auto p_buff = CreateBuffer(
			in_p_dev,
			// 縦横4x4
			4, 4,
			// RGBAの4byte
			DXGI_FORMAT_R8G8B8A8_UNORM,
			1,
			1,
			// 2Dテクスチャ
			D3D12_RESOURCE_DIMENSION_TEXTURE2D);
		assert(p_buff != nullptr);

		// 縦横4x4のRGBAの4byteのデータを作り、そのデータを0xffに埋めた白色データを作る
		// UINT8にしているのは単色値(1byte)で一括書き込みを実現するため
		std::vector<UINT8> data(4 * 4 * 4);
		std::fill(data.begin(), data.end(), in_color);

		// テクスチャバッファに白色データを書き込み
		auto h_result = p_buff->WriteToSubresource(
			0,
			nullptr,
			data.data(),
			// 1行分のデータ幅
			4 * 4,
			data.size()
		);
		assert(SUCCEEDED(h_result));

		out_p_texture->p_buf = p_buff;
		return true;
	}

	/// <summary>
	/// 単色のグラデーションテクスチャリソースを作成
	/// </summary>
	/// <param name="out_p_texture"></param>
	/// <param name="in_p_dev"></param>
	/// <param name="in_color"></param>
	/// <returns></returns>
	static bool CreateTextureResourceFromGradation(Resource* out_p_texture, ID3D12Device* in_p_dev, UINT8 in_color)
	{
		auto p_buff = CreateBuffer(
			in_p_dev,
			// 縦横4x256
			4, 256,
			// RGBAの4byte
			DXGI_FORMAT_R8G8B8A8_UNORM,
			1,
			1,
			// 2Dテクスチャ
			D3D12_RESOURCE_DIMENSION_TEXTURE2D);
		assert(p_buff != nullptr);

		// 縦横4x256のRGBAの4byteのデータを作り、そのデータを0xffに埋めた白色データを作る
		// 縦を256にしているのは色値は0 - 255になっているので0 - 255階調が作れる
		std::vector<UINT32> data(4 * 256);
		// 色のグラデーションを書き込む
		{
			UINT32 c = static_cast<UINT32>(in_color);
			for (auto it = data.begin(); it != data.end(); it += 4)
			{
				UINT32 write_color = (c << 24) | (c << 16) | (c << 8) | c;
				std::fill(it, it + 4, write_color);

				--c;
			}
		}

		// テクスチャバッファに色データを書き込み
		auto h_result = p_buff->WriteToSubresource(
			0,
			nullptr,
			data.data(),
			// 横のサイズ
			4 * sizeof(UINT32),
			data.size() * sizeof(UINT32)
		);
		assert(SUCCEEDED(h_result));

		out_p_texture->p_buf = p_buff;
		return true;
	}

	/// <summary>
	/// テクスチャファイルパスからファイルロードして作成バッファを返す
	/// </summary>
	/// <param name="in_r_file_path"></param>
	/// <returns></returns>
	static bool CreateTextureResourceFromLoadTextureFile(Resource* out_p_texture, ID3D12Device* in_p_dev, const std::string& in_r_file_path)
	{
		auto it = _s_cache_texture_resoucre.find(in_r_file_path);
		if (it != _s_cache_texture_resoucre.end())
		{
			out_p_texture->p_buf = it->second;
			return true;
		}

		DirectX::TexMetadata metadata = {};
		DirectX::ScratchImage scratch_img = {};

		auto result = LoadImageFile(&metadata, &scratch_img, in_r_file_path);
		assert(result);

		auto p_buff = CreateBuffer(
			in_p_dev,
			metadata.width, metadata.height,
			metadata.format,
			static_cast<UINT16>(metadata.arraySize),
			static_cast<UINT16>(metadata.mipLevels),
			static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension));
		assert(p_buff != nullptr);

		auto p_img = scratch_img.GetImage(0, 0, 0);
		auto h_result = p_buff->WriteToSubresource(
			0,
			nullptr,
			p_img->pixels,
			p_img->rowPitch,
			p_img->slicePitch
		);
		assert(SUCCEEDED(h_result));

		out_p_texture->p_buf = p_buff;

		_s_cache_texture_resoucre[in_r_file_path] = p_buff;

		return true;
	}
}