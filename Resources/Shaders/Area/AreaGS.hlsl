#include "Area.hlsli"

//	4つの頂点を作成する
static const int vnum = 4;

//	各頂点のUV
static const float2 UV[vnum] =
{
    float2(0, 0), //	左上
	float2(1, 0), //	右上
	float2(0, 1), //	左下
	float2(1, 1), //	右下
};

[maxvertexcount(vnum)]
void main(
	line GS_INPUT input[2],
	inout TriangleStream<PS_INPUT> output)
{
	//// ワールド行列を反映
	//float3 wPos0 = mul(float4(input[0].Pos), matWorld).xyz;
	//float3 wPos1 = mul(float4(input[1].Pos), matWorld).xyz;
	
	// 頂点座標を4つ作成
    float3 bottom0 = input[0].PosWS.xyz;
    float3 bottom1 = input[1].PosWS.xyz;
    float3 top0 = input[0].PosWS.xyz + float3(0, height, 0);
    float3 top1 = input[1].PosWS.xyz + float3(0, height, 0);
	
	// for文で回すための配列
    float3 pos[4] = { top0, top1, bottom0, bottom1 };
	
    [unroll] for (int i = 0; i < vnum; i++)
    {
		// 頂点データを作成
        PS_INPUT vertex;
        float4 vPos = mul(float4(pos[i], 1.0f), matView);
        vertex.Pos = mul(vPos, matProj);
        vertex.Color = input[0].Color;
        vertex.UV = UV[i];
		
		// 頂点データを登録
        output.Append(vertex);
    }
	
	// 生成した頂点データから、ポリゴン情報を適切に判断してもらう
    output.RestartStrip();
}