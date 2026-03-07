/**
 * @file   Button.cpp
 *
 * @brief  ボタンUIに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Button.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
Button::Button()
	: UIElement()
{

}



/**
 * @brief デストラクタ
 */
Button::~Button()
{

}



/**
 * @brief 初期化処理
 *
 * @param texture	テクスチャのポインタ
 * @param data		トゥイーン情報
 * @param size		テクスチャのサイズ
 * @param operate	押された時の処理
 *
 * @return なし
 */
void Button::Initialize(
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture,
	const Tween2D::TweenData& data,
	const DirectX::SimpleMath::Vector2& size,
	std::function<void()> operate)
{
	// ウィジェットの作成
	m_widget = std::make_unique<UIWidget>();
	m_widget->Initialize(texture, data, size);

	// 押された時の処理を設定
	m_operate = operate;
}



/**
 * @brief 更新処理
 *
 * @param なし
 *
 * @return なし
 */
void Button::Update(float elapsedTime)
{
	// ウィジェットの更新
	m_widget->Update(elapsedTime);
}



/**
 * @brief 描画処理
 *
 * @param なし
 *
 * @return なし
 */
void Button::Draw(const RenderContext& context)
{
	// ウィジェットの描画
	m_widget->Draw(context.spriteBatch);
}



/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void Button::Finalize()
{
	if(m_widget) m_widget->Finalize();
	m_widget.reset();

	m_operate = nullptr;
}



/**
 * @brief 押された時の処理
 *
 * @param なし
 *
 * @return なし
 */
void Button::Press()
{
	if (m_operate) m_operate();
}



/**
 * @brief トゥイーンアニメーションのリセット
 *
 * @param なし
 *
 * @return なし
 */
void Button::Reset()
{
	m_widget->TweenReset();
}



/**
 * @brief テクスチャの設定
 *
 * @param texture テクスチャのポインタ
 * @param size	  テクスチャのサイズ (同じ場合は入力無し)
 *
 * @return なし
 */
void Button::SetTexture(
	ID3D11ShaderResourceView* texture,
	const DirectX::SimpleMath::Vector2& size)
{
	// テクスチャを設定
	m_widget->SetTexture(texture);

	// サイズに変化があった場合は設定
	if (size != DirectX::SimpleMath::Vector2::Zero)
	{
		m_widget->SetTexSize(size);
	}
}
