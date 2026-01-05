#include "Area.hlsli"

GS_INPUT main(VS_INPUT input)
{
    GS_INPUT output;

    output.PosSV = float4(0, 0, 0, 1);
	
	//	ピクセルシェーダに渡す座標は、入力そのまま
    output.PosWS = input.Pos;

	//	色も指定値を使う
	output.Color = input.Color;
	
	//	次のシェーダへ情報を渡す
	return output;
}