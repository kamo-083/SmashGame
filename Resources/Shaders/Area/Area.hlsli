//	共通で必要となる構造体を、ヘッダファイルとして抜き出し
cbuffer ConstBuffer : register(b0)
{
    matrix matWorld;
    matrix matView;
    matrix matProj;
    float4 diffuse;
    float height;
    float3 start;
    float3 end;
    float dummy;
};

struct VS_INPUT
{
    float4 Color : COLOR;
	float3 Pos : POSITION;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float4 Color: COLOR;
};