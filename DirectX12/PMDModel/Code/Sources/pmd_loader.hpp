#pragma once

#include <stdio.h>
#include <vector>

// DXをインクルード
#include <d3d12.h>

namespace PMDLoader
{
	// PMDヘッダー構造体
	struct PMDHeader
	{
		// バージョン値
		float version;
		// モデル名
		char model_name[20];
		// モデルコメント
		char comment[256];
	};

	// PMD頂点構造体
	struct PMDVertex
	{
		// 頂点座標: 12byte
		DirectX::XMFLOAT3 pos;
		// 法線ベクトル: 12byte
		DirectX::XMFLOAT3 normal;
		// uv座標: 8byte
		DirectX::XMFLOAT2 uv;
		// ボーン番号: 4byte
		UINT16 bone_no[2];
		// ボーン影響度: 1byte
		UINT8 bone_weight;
		// 輪郭線フラグ: 1byte
		UINT8 edge_flag;
	};

	// PMDの関連データ一式
	struct PMDDataPack
	{
		PMDHeader header = {};
		std::vector<UINT8> vertexs;
		std::vector<UINT16> indices;
		size_t vertex_size = 0;
		UINT32 vert_num = 0;
	};

	/// <summary>
	/// PMDファイルからデータロード
	/// </summary>
	/// <param name="out_p_header"></param>
	/// <param name="in_p_pmd_filepath"></param>
	/// <returns></returns>
	static errno_t LoadFile(
		PMDDataPack* out_p_data_pack,
		const char* in_p_pmd_filepath)
	{
		FILE* fp = nullptr;
		auto error = fopen_s(&fp, in_p_pmd_filepath, "rb");
		if (error != 0)
			return error;

		// シグネチャーロード
		error = 1;
		{
			char signature[3] = {};
			if (fread(signature, sizeof(signature), 1, fp) == 0)
			{
				fclose(fp);
				return error;
			}
		}

		// TODO: シグネチャー名が正しいかのチェックは必要かな

		// ヘッダーをロード
		error = 2;
		{
			if (fread(&out_p_data_pack->header, sizeof(PMDHeader), 1, fp) == 0)
			{
				fclose(fp);
				return error;
			}
		}

		// 頂点データ群をロード
		error = 3;
		{
			constexpr size_t pmdvertex_size = 38;
			out_p_data_pack->vertex_size = pmdvertex_size;

			// 頂点数
			UINT32 vert_num = 0;
			if (fread(&vert_num, sizeof(vert_num), 1, fp) == 0)
			{
				fclose(fp);
				return error;
			}
			out_p_data_pack->vert_num = vert_num;

			// 頂点データ一覧を取得
			out_p_data_pack->vertexs.resize(vert_num * pmdvertex_size);
			if (fread(out_p_data_pack->vertexs.data(), out_p_data_pack->vertexs.size() * sizeof(out_p_data_pack->vertexs[0]), 1, fp) == 0)
			{
				fclose(fp);
				return error;
			}

			// インデックス数
			UINT32 indices_num = 0;
			if (fread(&indices_num, sizeof(indices_num), 1, fp) == 0)
			{
				fclose(fp);
				return error;
			}
			out_p_data_pack->indices.resize(indices_num);

			// インデックスデータ一覧を取得
			if (fread(out_p_data_pack->indices.data(), out_p_data_pack->indices.size() * sizeof(out_p_data_pack->indices[0]), 1, fp) == 0)
			{
				fclose(fp);
				return error;
			}
		}

		fclose(fp);

		error = 0;
		return error;
	}
}
