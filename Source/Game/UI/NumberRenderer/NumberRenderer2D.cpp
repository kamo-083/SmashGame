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
 * @param spriteSize	スプライトのサイズ
 * @param texture		テクスチャのポインタ
 * @param digit			表示桁数
 */
NumberRenderer2D::NumberRenderer2D(
	DirectX::SimpleMath::Vector2 spriteSize,
	ID3D11ShaderResourceView* texture,
	int digit)
	: INumberRenderer(spriteSize, texture, digit)
	, m_position{ DirectX::SimpleMath::Vector2::Zero }
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
void NumberRenderer2D::Draw(RenderContext& renderContext)
{
	// 表示数字を取得
	int data = m_number;

	// 表示位置を計算
	float x = m_position.x + NUM_DIGIT * SPRITE_SIZE.x;
	float y = m_position.y;

	renderContext.spriteBatch->Begin();

	// 1の位から順に描画
	for (int i = 0; i < NUM_DIGIT; i++)
	{
		int num = data % 10;
		int sourceX = num * static_cast<int>(SPRITE_SIZE.x);
		DirectX::SimpleMath::Vector2 pos = { x,y };
		RECT rect = {
			sourceX, 0,
			sourceX + static_cast<LONG>(SPRITE_SIZE.x), static_cast<LONG>(SPRITE_SIZE.y) };
		DirectX::FXMVECTOR color = DirectX::Colors::White;

		renderContext.spriteBatch->Draw(m_texture, pos, &rect,
			color, 0.0f, DirectX::XMFLOAT2(0, 0),
			1.0f, DirectX::SpriteEffects_None, 0.0f);

		data /= 10;
		x -= SPRITE_SIZE.x;
	}

	renderContext.spriteBatch->End();
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
