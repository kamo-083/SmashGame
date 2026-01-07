#pragma once
#include <d3d11_1.h>
#include "SimpleMath.h"

// テクスチャの情報
struct TextureInfo
{
	ID3D11ShaderResourceView* texture = nullptr;	// ポインタ
	DirectX::SimpleMath::Vector2 size;				// 画像サイズ
};
