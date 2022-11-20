#include "PeraShaderHeader.hlsli"

float4 BasicPS(Output input) : SV_TARGET
{
    //return 1.0 - tex.Sample(smp, input.uv);
    return tex.Sample(smp, input.uv);
}
