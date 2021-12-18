// 頂点シェーダーとピクセルシェーダーへのやり取りに使用する構造体
struct Output
{
	// 頂点情報
	float4 svpos : SV_POSITION;
	// 法線情報
	float4 normal : NORMAL;
	// uv情報
	float2 uv : TEXCOORD;
};

// 定数バッファ
cbuffer cbuff0 : register(b0)
{
	// 座標変換行列
	matrix world_mat;
	matrix viewproj_mat;
}
