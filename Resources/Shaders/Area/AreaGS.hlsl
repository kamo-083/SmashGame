#include "Area.hlsli"

//	4つの頂点を作成する
static const int vnum = 4;

//	各頂点のベースとする情報
static const float4 offset_array[vnum] =
{
	float4(-0.5f,  0.5f, 0.0f, 0.0f),	//	左上
	float4( 0.5f,  0.5f, 0.0f, 0.0f),	//	右上
	float4(-0.5f, -0.5f, 0.0f, 0.0f),	//	左下
	float4( 0.5f, -0.5f, 0.0f, 0.0f),	//	右下

};

[maxvertexcount(vnum)]
void main(
	point PS_INPUT input[1],
	inout TriangleStream< PS_INPUT > output
)
{
	// 4つの頂点を生成
	// 座標を設定(左上)
    PS_INPUT leftTop;
    leftTop.Pos = float4(start.x, height, start.z, 1);
    leftTop.Pos = mul(leftTop.Pos, matWorld);
    leftTop.Pos = mul(leftTop.Pos, matView);
    leftTop.Pos = mul(leftTop.Pos, matProj);
    leftTop.Color = input[0].Color;
	// 頂点情報を追加
    output.Append(leftTop);
	
	// 座標を設定(右上)
    PS_INPUT rightTop;
    rightTop.Pos = float4(end.x, end.y + height, end.z, 1);
    rightTop.Pos = mul(rightTop.Pos, matWorld);
    rightTop.Pos = mul(rightTop.Pos, matView);
    rightTop.Pos = mul(rightTop.Pos, matProj);
    rightTop.Color = input[0].Color;
	// 頂点情報を追加
    output.Append(rightTop);
	
	// 座標を設定(左下)
    PS_INPUT leftBottom;
    leftBottom.Pos = float4(start.x, start.y, start.z, 1);
    leftBottom.Pos = mul(leftBottom.Pos, matWorld);
    leftBottom.Pos = mul(leftBottom.Pos, matView);
    leftBottom.Pos = mul(leftBottom.Pos, matProj);
    leftBottom.Color = input[0].Color;
	// 頂点情報を追加
    output.Append(leftBottom);
	
	// 座標を設定(右下)
    PS_INPUT rightBottom;
    rightBottom.Pos = float4(end.x, end.y, end.z, 1);
    rightBottom.Pos = mul(rightBottom.Pos, matWorld);
    rightBottom.Pos = mul(rightBottom.Pos, matView);
    rightBottom.Pos = mul(rightBottom.Pos, matProj);
    rightBottom.Color = input[0].Color;
	// 頂点情報を追加
    output.Append(rightBottom);
	
	// 生成した頂点データから、ポリゴン情報を適切に判断してもらう
	output.RestartStrip();
}