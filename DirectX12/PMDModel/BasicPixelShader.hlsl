#include "BasicShaderHeader.hlsli"

float4 BasicPS(Output input) : SV_TARGET
{
	// ランバートの余弦測でライティング計算
	float3 light = normalize(float3(1.0, -1.0, 1.0));
	float b = dot(-light, input.normal);

	// 一旦黒色に
	return float4(b, b, b, 1);
}
