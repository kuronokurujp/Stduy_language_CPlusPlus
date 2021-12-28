// 頂点シェーダーとピクセルシェーダーへのやり取りに使用する構造体
struct Output
{
    // 頂点情報
    float4 svpos : SV_POSITION;
    // 頂点座標
    float4 pos : POSITION;
    // 法線情報
    float4 normal : NORMAL0;
    // View反映した法線情報
    float4 vnormal : NORMAL1;
    // uv情報
    float2 uv : TEXCOORD;
    // 視線ベクトル
    float3 ray : VECTOR;
};

cbuffer SceneBuffer : register(b0)
{
    // 座標変換行列
    matrix view_mat;
    matrix proj_mat;
    // カメラの視点
    float3 eye;
};

// ワールド座標変換
cbuffer TranformBuffer : register(b1)
{
    // ワールド変換行列
    matrix world_mat;
    // ボーン行列
    matrix bone_mats[256];
};

cbuffer Material : register(b2)
{
    // 拡散反射
    float4 diffuse;
    // 鏡面反射
    float4 specular;
    // 環境光
    float3 ambient;
};

// 0番スロットに設定テクスチャ
Texture2D<float4> tex : register(t0);
// 1番スロットにスフィアテクスチャ
Texture2D<float4> sph : register(t1);
// 2番スロットに加算スフィアテクスチャ
Texture2D<float4> spa : register(t2);
// 3番スロットにトゥーンテクスチャ
Texture2D<float4> toon : register(t3);

// 0番スロットに設定サンプラー
SamplerState smp : register(s0);
// 1番スロットに設定サンプラー
SamplerState toon_smp : register(s1);
