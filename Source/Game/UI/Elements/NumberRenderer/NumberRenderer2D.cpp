/**
 * @file   NumberRenderer2D.cpp
 *
 * @brief  2Dのスプライト数字に関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "NumberRenderer2D.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param mode			表示形式
 * @param spriteSize	スプライトのサイズ
 * @param texture		テクスチャのポインタ
 * @param digit			表示桁数
 */
NumberRenderer2D::NumberRenderer2D(
	DisplayMode mode,
	DirectX::SimpleMath::Vector2 spriteSize,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture,
	int digit)
	: 
	INumberRenderer(mode, spriteSize, texture, digit),
	m_position{ DirectX::SimpleMath::Vector2::Zero },
	m_useBeginEnd{ false },
	m_scale{ 1.0f }
{
}



/**
 * @brief デストラクタ
 */
NumberRenderer2D::~NumberRenderer2D()
{

}



/**
 * @brief 初期化処理
 *
 * @param number	表示数字
 *
 * @return なし
 */
void NumberRenderer2D::Initialize(const int& number)
{
	m_number = number;
}



/**
 * @brief 描画処理
 *
 * @param renderContext	描画用構造体
 *
 * @return なし
 */
void NumberRenderer2D::Draw(const RenderContext& renderContext)
{
	// 表示位置を取得
	float x = m_position.x;
	float y = m_position.y;
	// スプライトサイズの半分
	DirectX::SimpleMath::Vector2 halfSize = SPRITE_SIZE * 0.5f;

	if(m_useBeginEnd) renderContext.spriteBatch->Begin();

	switch (m_mode)
	{
	case INumberRenderer::DisplayMode::Default:		// 通常
		// 数字を描画
		DrawNumber(
			m_number, x, y, m_scale, halfSize,
			renderContext.spriteBatch);
		break;
	case INumberRenderer::DisplayMode::Fraction:	// 分数
		float base_x = x;
		// スラッシュを描画
		DrawSlash(
			base_x, y, m_scale, halfSize,
			renderContext.spriteBatch);

		// 表示位置を計算
		x = base_x + NUM_DIGIT * SPRITE_SIZE.x;
		// 分母を描画
		DrawNumber(
			m_fraction.denominator, x, y, m_scale, halfSize,
			renderContext.spriteBatch);

		// 表示位置を計算
		x = base_x - SPRITE_SIZE.x;
		// 分子を描画
		DrawNumber(
			m_fraction.numerator, x, y, m_scale, halfSize,
			renderContext.spriteBatch);
		break;
	}

	if (m_useBeginEnd) renderContext.spriteBatch->End();
}



/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void NumberRenderer2D::Finalize()
{
	m_texture = nullptr;
}