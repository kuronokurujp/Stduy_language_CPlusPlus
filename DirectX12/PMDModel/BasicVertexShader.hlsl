#include "BasicShaderHeader.hlsli"

Output BasicVS(
	float4 pos : POSITION,
	float4 normal : NORMAL,
	float2 uv : TEXCOORD,
	min16uint2 boneno : BONE_NO,
	min16uint weight : WEIGHT)
{
	Output output;
	output.svpos = mul(mul(viewproj_mat, world_mat), pos);
	// 平行移動成分を消して回転とスケール要素のみ反映している
	// この方法だとスケール要素も反映して法線の長さが変わるが、正規化しているので問題ない
	normal.w = 0;
	output.normal = mul(world_mat, normal);
	output.uv = uv;
	return output;
}
