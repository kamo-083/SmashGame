/**
 * @file   INumberRenderer.h
 *
 * @brief  スプライトを使用した数字描画に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Source/Game/Common/RenderContext.h"


// クラスの定義 ===============================================================
/**
 * @brief スプライトを使用した数字描画
 */
class INumberRenderer
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// 表示形式
	enum class DisplayMode
	{
		Default,	// 通常
		Fraction	// 分数
	};

protected:
	// 1桁のスプライトの大きさ
	const DirectX::SimpleMath::Vector2 SPRITE_SIZE;

	// 桁数
	const int NUM_DIGIT;

	// 分数表示の数値
	struct Fraction
	{
		int denominator;	// 分母
		int numerator;		// 分子
	};

	// データメンバの宣言 -----------------------------------------------
protected:
	// 表示形式
	DisplayMode m_mode;

	// 表示する数字
	int m_number;
	
	// 表示する分数
	Fraction m_fraction;

	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;


	// メンバ関数の宣言 -------------------------------------------------
public:
	/**
	 * @brief コンストラクタ
	 *
	 * @param mode			表示形式
	 * @param spriteSize	スプライトのサイズ
	 * @param texture		テクスチャ
	 * @param digit			表示する桁数
	 */
	INumberRenderer(
		DisplayMode mode,
		const DirectX::SimpleMath::Vector2& spriteSize,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture, 
		int digit)
		: SPRITE_SIZE(spriteSize)
		, NUM_DIGIT(digit)
		, m_mode(mode)
		, m_texture(texture)
		, m_number(0)
		, m_fraction()
	{}

	/**
	 * @brief デストラクタ
	 */
	virtual ~INumberRenderer()
	{
	}

public:
	// 初期化処理
	virtual void Initialize(const int& number) = 0;

	// 描画処理
	virtual void Draw(const RenderContext& renderContext) = 0;

	// 終了処理
	virtual void Finalize() = 0;

	/**
	 * @brief 表示する数字を設定
	 *
	 * @param number	表示数字
	 * 
	 * @return	なし
	 */
	void SetNumber(const int& number)
	{
		m_number = number; 
	}
	
	/**
	 * @brief 表示する分数を設定
	 *
	 * @param numerator		表示分子
	 * @param denominator	表示分母
	 * 
	 * @return	なし
	 */
	void SetFraction(const int& numerator, const int& denominator = INT16_MAX)
	{
		// 分子を設定
		m_fraction.numerator = numerator;

		// 分母の入力があった場合は設定
		if (denominator != INT16_MAX)	m_fraction.denominator = denominator;
	}

	/**
	 * @brief スプライトの大きさを取得
	 *
	 * @param なし
	 * 
	 * @return	スプライトの大きさ
	 */
	DirectX::SimpleMath::Vector2  GetSpriteSize() const
	{
		return SPRITE_SIZE;
	}

	/**
	 * @brief 表示形式を設定
	 *
	 * @param mode	表示形式
	 * 
	 * @return	なし
	 */
	virtual void SetDisplayMode(INumberRenderer::DisplayMode mode)
	{
		m_mode = mode;
	}

	/**
	 * @brief 数字を描画
	 *
	 * @param data			表示する数字
	 * @param x				X座標
	 * @param y				Y座標
	 * @param scale			大きさの倍率
	 * @param origin		画像の中心
	 * @param spriteBatch	スプライトバッチのポインタ
	 *
	 * @return	なし
	 */
	void DrawNumber(
		int data,
		float x, float y,
		const float scale,
		const DirectX::SimpleMath::Vector2& origin,
		DirectX::SpriteBatch* spriteBatch)
	{
		// 1の位から順に描画
		for (int i = 0; i < NUM_DIGIT; i++)
		{
			int num = data % 10;
			int sourceX = num * static_cast<int>(SPRITE_SIZE.x);
			// 表示位置
			DirectX::SimpleMath::Vector2 pos = { x, y };
			// 切り取り範囲
			RECT rect = {
				sourceX, 0,
				sourceX + static_cast<LONG>(SPRITE_SIZE.x), static_cast<LONG>(SPRITE_SIZE.y) };
			DirectX::FXMVECTOR color = DirectX::Colors::White;

			// 数字を描画
			spriteBatch->Draw(
				m_texture.Get(), pos, &rect,
				color, 0.0f, origin,
				scale, DirectX::SpriteEffects_None, 0.0f);

			// 1桁分ずらす
			data /= 10;
			x -= SPRITE_SIZE.x * scale;
		}
	}

	/**
	 * @brief スラッシュを描画
	 *
	 * @param x				X座標
	 * @param y				Y座標
	 * @param scale			大きさの倍率
	 * @param origin		画像の中心
	 * @param spriteBatch	スプライトバッチのポインタ
	 *
	 * @return	なし
	 */
	void DrawSlash(
		float x, float y,
		const float scale,
		const DirectX::SimpleMath::Vector2& origin,
		DirectX::SpriteBatch* spriteBatch)
	{
		// 表示位置
		DirectX::SimpleMath::Vector2 pos = { x, y };
		// 切り取り範囲
		RECT rect = {
			0, static_cast<LONG>(SPRITE_SIZE.y),
			static_cast<LONG>(SPRITE_SIZE.x), static_cast<LONG>(SPRITE_SIZE.y * 2.0f) };
		DirectX::FXMVECTOR color = DirectX::Colors::White;

		// スラッシュを描画
		spriteBatch->Draw(
			m_texture.Get(), pos, &rect,
			color, 0.0f, origin,
			scale, DirectX::SpriteEffects_None, 0.0f);
	}
};
