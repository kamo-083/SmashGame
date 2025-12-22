#include "Transition.hlsli"

Texture2D tex : register(t0);
SamplerState samLinear : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{	
    float mask = tex.Sample(samLinear, input.Tex).r; // –¾‚é‚³‚¾‚¯Žæ‚é
    
    float t = step(mask, rate); // 0 or 1

    return float4(0, 0, 0, t);
}