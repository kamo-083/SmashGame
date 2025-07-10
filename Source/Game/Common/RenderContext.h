/**
 * @file   RenderContext.h
 *
 * @brief  描画に使う共通の行列などの構造体に関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include"pch.h"



// 構造体の定義 ===============================================================
/**
 * @brief 描画に使う共通の行列などの構造体
 */
struct RenderContext
{
	// 3D
	DirectX::SimpleMath::Matrix view;		//ビュー行列
	DirectX::SimpleMath::Matrix projection;	//射影行列
	ID3D11DeviceContext* deviceContext;		//デバイスコンテキスト
	DirectX::CommonStates* states;			//共通ステート

	// 2D
	DirectX::SpriteBatch* spriteBatch;		//スプライトバッチ
};
