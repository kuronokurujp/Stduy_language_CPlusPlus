#pragma once

#include <string>
#include <assert.h>
#include <stringapiset.h>

namespace Common
{
	/// <summary>
	/// ファイル名から拡張子を取得
	/// </summary>
	/// <param name="in_r_filename"></param>
	/// <returns></returns>
	std::string GetFileExtension(const std::string& in_r_filename)
	{
		auto idx = in_r_filename.rfind('.');
		return in_r_filename.substr(idx + 1, in_r_filename.length() - idx - 1);
	}

	/// <summary>
	/// 文字列内に区切り文字があった場合に2つの文字列に分割する
	/// 区切り文字が1つしかない想定なので注意
	/// </summary>
	/// <param name="in_r_filename"></param>
	/// <param name="in_spliter"></param>
	/// <returns></returns>
	std::pair<std::string, std::string> SplitFileName(const std::string& in_r_filename, const char in_spliter = '*')
	{
		int idx = in_r_filename.find(in_spliter);

		auto first = in_r_filename.substr(0, idx);
		auto second = in_r_filename.substr(idx + 1, in_r_filename.length() - idx - 1);

		return std::pair<std::string, std::string>(first, second);
	}

	/// <summary>
	/// stringの文字列からwstringの文字列に変換したのを取得
	/// </summary>
	/// <param name="in_r_str"></param>
	/// <returns></returns>
	static std::wstring GetWideStringFromString(const std::string& in_r_str)
	{
		// wstringの文字列数を取得
		auto num_1 = MultiByteToWideChar(
			CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			in_r_str.c_str(),
			-1,
			nullptr,
			0);

		// 文字列数からwstringの文字列を作成
		std::wstring conv_string;
		conv_string.resize(num_1);

		// string to wstringにする
		auto num_2 = MultiByteToWideChar(
			CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			in_r_str.c_str(),
			-1,
			&conv_string[0],
			num_1);

		assert(num_1 == num_2);

		return conv_string;
	}

	/// <summary>
	/// ディレクトリパスの末尾にパスを追加したパスを返す
	/// 指定パスをディレクトリパスに変換
	/// そのディレクトリパスの末尾に引数で指定したパスを追加
	/// </summary>
	/// <param name="in_currnet_path"></param>
	/// <param name="in_p_append_path"></param>
	/// <returns></returns>
	static std::string GetPathForAddPathToDirectoryEndPath(
		const std::string& in_currnet_path,
		const char* in_p_append_path)
	{
		// windowsだとパス区切りが/ or \と2種類あるのでどちらでも対応するようにしている
		INT path_index1 = in_currnet_path.rfind('/');
		INT path_index2 = in_currnet_path.rfind('\\');

		auto path_index = max(path_index1, path_index2);

		// 区切りの文字は残す
		auto dir_path = in_currnet_path.substr(0, path_index + 1);
		// パス同士の結合
		return dir_path + in_p_append_path;
	}

	/// <summary>
	/// エラーのBLOBからエラーの具体的な内容を文字列で出力
	/// </summary>
	/// <param name="out_p_error"></param>
	/// <param name="in_result"></param>
	/// <param name="in_p_error_blob"></param>
	static void OutputErrorBlob(std::string* out_p_error, const HRESULT in_result, ID3DBlob* const in_p_error_blob)
	{
		if (!FAILED(in_result))
			return;

		if (in_result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			if (out_p_error != nullptr)
				out_p_error->append("shader file find not");

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

	/// <summary>
	/// 設定した値をアライメント幅に合わせて返している
	/// </summary>
	/// <param name="in_size"></param>
	/// <param name="in_alignment"></param>
	/// <returns></returns>
	static size_t AlignmentedSize(const size_t in_size, const size_t in_alignment)
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
		const UINT in_descriptors_num)
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
