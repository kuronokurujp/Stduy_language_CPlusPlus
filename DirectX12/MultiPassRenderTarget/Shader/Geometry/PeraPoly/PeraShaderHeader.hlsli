// 通常テクスチャ
Texture2D<float4> tex : register(t0);
// サンプラー
SamplerState smp : register(s0);

// 頂点出力
struct Output
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};
