#include "Area.hlsli"

// ベースの透明度
static const float BASE_ALPHA = 0.6f;
// 時間による変動
static const float ALPHA_MAX = 1.5f;
static const float ALPHA_MIN = 1.0f;
// スキャンライン関連
static const float SCANLINE_UV_SCALE = 15.0f;
static const float SCANLINE_SPEED = 3.0f;
static const float SCANLINE_MAX = 1.2f;
static const float SCANLINE_MIN = 0.7f;
// グラデーション関連
static const float GRADATION_POWER = 0.65f;

// スキャンライン
float ScanLine(float2 uv)
{
    float scan = sin((uv.y * SCANLINE_UV_SCALE) + time * SCANLINE_SPEED);
    scan = saturate(scan); // 0～1に収める
     
    return lerp(SCANLINE_MIN, SCANLINE_MAX, scan);
}

// グラデーション
float Gradation(float2 uv)
{    
    float g = saturate(uv.y / height);
    g = pow(g, GRADATION_POWER);
    
    return g;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    // 透明度を時間で変動
    float t = 0.5f + 0.5f * sin(time);  // 0～1
    float a = lerp(ALPHA_MIN, ALPHA_MAX, BASE_ALPHA + t);
    
    // 下から上へグラデーション
    a *= Gradation(input.UV);
            
    // ラインのアニメーション
    a += ScanLine(input.UV) - 1.0f;
        
	//	C++から指定された色割合を考慮して、表示色を決定
    return float4(input.Color.rgb, a);
}