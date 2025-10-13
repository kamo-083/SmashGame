#include "Area.hlsli"

static const float alpha = 0.25f;

float4 main(PS_INPUT input) : SV_TARGET
{
	//	C++から指定された色割合を考慮して、表示色を決定
    return float4(input.Color.rgb, alpha);
}