#include "Transition.hlsli"

Texture2D tex : register(t0);
SamplerState samLinear : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{	
    float mask = tex.Sample(samLinear, input.Tex).r; // –¾‚é‚³‚¾‚¯Žæ‚é
    
    float t = step(mask, rate); // 0 or 1

    return float4(0, 0, 0, t);
    
    //float4 color = tex.Sample(samLinear, input.Tex);
	
    //float4 fade = tex2.Sample(samLinear, input.Tex);
		
    //fade.a = ceil(fade.b - sin(time.x));
	
    //return float4(color.r, color.g, color.b, fade.a);
}