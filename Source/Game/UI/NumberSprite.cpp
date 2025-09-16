/**
 * @file   NumberSprite.cpp
 *
 * @brief  NumberSpriteに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "NumberSprite.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
NumberSprite::NumberSprite(DirectX::SimpleMath::Vector2 spriteSize,
						   ID3D11ShaderResourceView* texture,
						   int digit)
	: SPRITE_SIZE{ spriteSize }
	, m_texture{ texture }
	, NUM_DIGIT{ digit }
	, m_number{ 0 }
{
}


/**
 * @brief デストラクタ
 */
NumberSprite::~NumberSprite()
{
	m_texture = nullptr;
}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void NumberSprite::Initialize(DirectX::SimpleMath::Vector2 position, const int& number)
{
	m_number = number;
	m_position = position;
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void NumberSprite::Draw(DirectX::SpriteBatch& spriteBatch)
{
	int data = m_number;
	float x = m_position.x + NUM_DIGIT * SPRITE_SIZE.x;
	float y = m_position.y;

	spriteBatch.Begin();

	for (int i = 0; i < NUM_DIGIT; i++)
	{
		int number = data % 10;
		int sourceX = number * SPRITE_SIZE.x;
		DirectX::SimpleMath::Vector2 pos = { x,y };
		RECT rect = { sourceX,0,sourceX + SPRITE_SIZE.x,SPRITE_SIZE.y };
		DirectX::FXMVECTOR color = DirectX::Colors::White;

		spriteBatch.Draw(m_texture, pos, &rect,
			color, 0.0f, DirectX::XMFLOAT2(0, 0),
			1.0f, DirectX::SpriteEffects_None, 0.0f);

		data /= 10;
		x -= SPRITE_SIZE.x;
	}

	spriteBatch.End();
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void NumberSprite::Finalize()
{
	m_texture = nullptr;
}
