//	共通で必要となる構造体を、ヘッダファイルとして抜き出し
cbuffer ConstBuffer : register(b0)
{
    matrix matWorld;
    matrix matView;
    matrix matProj;
    float4 diffuse;
    float height;
    float3 dummy;
};

struct VS_INPUT
{
    float4 Color : COLOR;
	float3 Pos : POSITION;
};

struct GS_INPUT
{
    float3 PosWS : POSITION;
    float4 Color : COLOR;
    float4 PosSV : SV_POSITION;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float4 Color: COLOR;
    float2 UV : TEXCOORD0;
};
