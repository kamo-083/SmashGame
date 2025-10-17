/**
 * @file   NumberRenderer2D.cpp
 *
 * @brief  NumberRenderer2Dに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "NumberRenderer2D.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
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
 * @param なし
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
 * @param なし
 *
 * @return なし
 */
void NumberRenderer2D::Draw(RenderContext& renderContext)
{
	int data = m_number;
	float x = m_position.x + NUM_DIGIT * SPRITE_SIZE.x;
	float y = m_position.y;

	renderContext.spriteBatch->Begin();

	for (int i = 0; i < NUM_DIGIT; i++)
	{
		int number = data % 10;
		int sourceX = number * SPRITE_SIZE.x;
		DirectX::SimpleMath::Vector2 pos = { x,y };
		RECT rect = { sourceX,0,sourceX + SPRITE_SIZE.x,SPRITE_SIZE.y };
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
