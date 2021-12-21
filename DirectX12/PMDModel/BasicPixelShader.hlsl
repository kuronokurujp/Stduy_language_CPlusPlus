#include "BasicShaderHeader.hlsli"

float4 BasicPS(Output input) : SV_TARGET
{
	// 光の向かうベクトル(平行光源からの平行な光で本来なら外部パラメータで渡すべき)
	float3 light = normalize(float3(1.0, -1.0, 1.0));

	// 光の反射ベクトルを作成
	// 法線と光りのベクトルとの反射ベクトルを作成
	float3 ref_light = normalize(reflect(light, input.normal.xyz));
	// 反射ベクトルと視線ベクトルで鏡面反射力を求める
	float specular_b = pow(saturate(dot(ref_light, -input.ray)), specular.a);
	// 鏡面反射係数と鏡面反射力を乗算
	float4 specular_v = float4(specular_b * specular.rgb, 1);

	// ランバートの余弦測の計算から輝度値を求める
	float b = saturate(dot(-light, input.normal.xyz));
	// 輝度値を反転したのをトゥーンテクスチャのUV値にする
	// これはトゥーンテクスチャのUV値が小さい範囲に明るい色があるから
	// もし逆にUV値が大きい箇所に明るい色があれば輝度値の反転はいらない
	// データ内容を把握しないと痛い目にあう
	// uv値のvは不要
	float4 toon_diff = toon.Sample(toon_smp, float2(0.0, 1.0 - b));

	float2 sphere_uv_map = input.vnormal.xy;
	// スフィアの法線をデカルト座標からスクリーン座標系に座標変換
	sphere_uv_map = (sphere_uv_map + float2(1.0, -1.0)) * float2(0.5, -0.5);

	// 環境項の補正用に用意
	float4 tex_color = tex.Sample(smp, input.uv);

	// 最終ピクセル色出力
	return max(
		// トゥーン
		toon_diff
		// 拡散反射光
		* diffuse
		// テクスチャ色
		* tex_color
		// 乗算用のスフィアマップ
		* sph.Sample(smp, sphere_uv_map)
		// 加算用のスフィアマップ
		+ spa.Sample(smp, sphere_uv_map) * tex_color
		// 鏡面反射を加算
		+ specular_v,
		// 環境光
		float4(ambient * tex_color.xyz, 1));
}
