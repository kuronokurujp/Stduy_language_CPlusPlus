#include "BasicShaderHeader.hlsli"

Output BasicVS(
    // 頂点レイアウトに剃った引数にする
    float4 pos : POSITION,
    float4 normal : NORMAL,
    float2 uv : TEXCOORD,
    min16uint2 boneno : BONE_NO,
    min16uint weight : WEIGHT)
{
    Output output;
    // ローカル座標からボーン行列の座標変換
    output.pos = mul(bone_mats[boneno], pos);

    // ローカル座標からワールド座標系の座標に
    output.pos = mul(world_mat, output.pos);

    // テクスチャで利用するUV
    output.uv = uv;

    // ローカル座標から正規化デバイス座標系の座標に
    output.svpos = mul(mul(proj_mat, view_mat), output.pos);

    // 平行移動成分を消して回転とスケール要素のみ反映している
    // この方法だとスケール要素も反映して法線の長さが変わるが、正規化しているので問題ない
    normal.w = 0;
    output.normal = mul(world_mat, normal);

    // カメラとワールドの座標変換を与えた法線
    // スフィアマップに利用
    output.vnormal = mul(view_mat, output.normal);

    // 視点から頂点のワールド座標の視線ベクトルを作成
    output.ray = normalize(output.pos.xyz - eye);

    return output;
}
