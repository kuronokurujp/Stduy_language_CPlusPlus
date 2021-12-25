#pragma once

#include <stdio.h>
#include <vector>

#include <DirectXMath.h>
// UINT32など型定義
#include <basetsd.h>

namespace PMD
{
	namespace Loader
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

#pragma pack(1)
		// マテリアルデータ
		// 70byteのデータ塊なのでアライメントを発生しないようにしている
		struct PMDMaterial
		{
			// 拡散反射光色
			DirectX::XMFLOAT3 diffuse;
			// 拡散反射の透明度
			float alpha;
			// 鏡面反射の強さ
			float specularity;
			// 鏡面反射の色
			DirectX::XMFLOAT3 specular;
			// 環境項の色
			DirectX::XMFLOAT3 ambiend;
			// トゥーン番号
			UINT8 toon_idx;
			// マテリアルごとの輪郭線フラグ
			UINT8 edge_flag;

			// 2byteのパディングができるのを防ぐ

			// マテリアルが割り当てられているインデックス数
			UINT32 indices_num;

			// テクスチャファイルパス
			char tex_file_path[20];
		};
#pragma pack()

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
		extern errno_t SyncLoadFile(PMDDataPack* out_p_data_pack, std::vector<PMDMaterial>* out_p_material_datas, const char* in_p_pmd_filepath);
	}
}
