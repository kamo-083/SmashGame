#include "Area.hlsli"

static const float alpha = 0.5f;
static const float larpMax = 1.0f;
static const float larpMin = 0.8f;

float4 main(PS_INPUT input) : SV_TARGET
{
    // 透明度を時間で変動
    float a = lerp(larpMin, larpMax, alpha + sin(time));    
    
    // 下から上へグラデーション
    a *= input.UV.y / height;
    
	//	C++から指定された色割合を考慮して、表示色を決定
    return float4(input.Color.rgb, a);
}