/**
 * @file   UIWidget.cpp
 *
 * @brief  UIWidgetに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "UIWidget.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
UIWidget::UIWidget()
	: m_texture{ nullptr }
{

}



/**
 * @brief デストラクタ
 */
UIWidget::~UIWidget()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void UIWidget::Initialize(ID3D11ShaderResourceView* texture,const Tween::TweenData data, DirectX::SimpleMath::Vector2 size)
{
	m_texture = texture;
	m_params = data.start;
	m_texSize = size;

	m_tween = std::make_unique<Tween>(data);

	m_tween->Play();
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void UIWidget::Update(float elapsedTime)
{
	m_tween->Update(elapsedTime, m_params);
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void UIWidget::Draw(RenderContext context)
{
	SimpleMath::Color color = { 1, 1, 1, m_params.opacity };
	SimpleMath::Vector2 size = { m_texSize.x * 0.5f,m_texSize.y * 0.5f };

	context.spriteBatch->Begin(
		SpriteSortMode_Deferred,
		context.states->NonPremultiplied(),
		context.states->LinearClamp()
	);

	context.spriteBatch->Draw(m_texture, m_params.pos, nullptr, color, m_params.rotation, size, m_params.scale, SpriteEffects_None, 0.0f);

	context.spriteBatch->End();
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void UIWidget::Finalize()
{
	m_texture = nullptr;

	m_tween->Finalize();
	m_tween.reset();
}
