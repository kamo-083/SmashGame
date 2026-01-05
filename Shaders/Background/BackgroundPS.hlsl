#include "Background.hlsli"

Texture2D tex : register(t0);
SamplerState samLinear : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{	    
    float2 uv = input.Tex;
    
    // 移動
    uv += (time * 0.1f) * moveDir;
    
    // アスペクト比
    float aspect = windowSize.x / windowSize.y;
    float2 aspect_uv = uv;
    aspect_uv.x *= aspect;    
    
    float4 output = tex.Sample(samLinear, aspect_uv);
    
    return output;
}