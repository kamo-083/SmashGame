/**
 * @file   INumberRenderer.h
 *
 * @brief  INumberRendererに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Source/Game/Common/RenderContext.h"


// クラスの定義 ===============================================================
/**
 * @brief INumberRenderer
 */
class INumberRenderer
{
	// クラス定数の宣言 -------------------------------------------------
protected:
	// 1桁のスプライトの大きさ
	const DirectX::SimpleMath::Vector2 SPRITE_SIZE;

	// 桁数
	const int NUM_DIGIT;


	// データメンバの宣言 -----------------------------------------------
protected:
	// 表示する数字
	int m_number;

	// テクスチャ
	ID3D11ShaderResourceView* m_texture;


	// メンバ関数の宣言 -------------------------------------------------
public:
	// コンストラクタ
	INumberRenderer(DirectX::SimpleMath::Vector2 spriteSize, ID3D11ShaderResourceView* texture, int digit)
		: SPRITE_SIZE(spriteSize)
		, NUM_DIGIT(digit)
		, m_texture(texture)
		, m_number(0)
	{}

	// デストラクタ
	~INumberRenderer()
	{
	}

public:
	// 初期化処理
	virtual void Initialize(const int& number) = 0;

	// 描画処理
	virtual void Draw(RenderContext& renderContext) = 0;

	// 終了処理
	virtual void Finalize() = 0;

	// 数値を設定
	void SetNumber(const int& number)
	{
		m_number = number; 
	}
};
