// 頂点シェーダーとピクセルシェーダーへのやり取りに使用する構造体
struct Output
{
	// 頂点情報
	float4 svpos : SV_POSITION;
	// uv情報
	float2 uv : TEXCOORD;
};

// 0番スロットに設定テクスチャ
Texture2D<float4> tex : register(t0);
// 0番スロットに設定サンプラー
SamplerState smp : register(s0);
