#include "Area.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
    float alpha = 1.0f;
    
	//	C++から指定された色割合を考慮して、表示色を決定
    return float4(input.Color.rgb, alpha);
}