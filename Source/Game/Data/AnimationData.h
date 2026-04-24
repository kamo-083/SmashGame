// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "pch.h"

// アニメーションのバイナリデータ
struct AnimationBinaryData
{
	std::unique_ptr<uint8_t[]>	animData;
	size_t						animSize = 0;
};