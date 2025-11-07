/**
 * @file   InputGuideUI.cpp
 *
 * @brief  ボタン操作UIに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "InputGuideUI.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
InputGuideUI::InputGuideUI()
	: UIElement()
{

}



/**
 * @brief デストラクタ
 */
InputGuideUI::~InputGuideUI()
{

}



/**
 * @brief 初期化処理
 *
 * @param texture	テクスチャのポインタ
 * @param pos		表示位置
 * @param size		テクスチャのサイズ
 *
 * @return なし
 */
void InputGuideUI::Initialize(
	ID3D11ShaderResourceView* texture,
	DirectX::SimpleMath::Vector2 pos,
	DirectX::SimpleMath::Vector2 size)
{
	Tween2D::TweenData data =
	{
		Tween2D::UIParams{pos,DirectX::SimpleMath::Vector2(1.0f,1.0f), 0.0f, 1.0f},
		Tween2D::UIParams{DirectX::SimpleMath::Vector2(0.0f),
						  DirectX::SimpleMath::Vector2(- 0.2f),
						  0.0f, -0.2f},
		0.5f,
		Tween2D::Ease::OutQuart,
		Tween2D::PlaybackMode::Once
	};
	// ウィジェットの作成
	m_widget = std::make_unique<UIWidget>();
	m_widget->Initialize(texture, data, size, false);
}



/**
 * @brief 更新処理
 *
 * @param なし
 *
 * @return なし
 */
void InputGuideUI::Update(float elapsedTime)
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
void InputGuideUI::Draw(RenderContext context)
{
	// ウィジェットの描画
	m_widget->Draw(context);
}



/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void InputGuideUI::Finalize()
{
	if (m_widget) m_widget->Finalize();
	m_widget.reset();
}



/**
 * @brief 押された時の処理
 *
 * @param なし
 *
 * @return なし
 */
void InputGuideUI::Press()
{
	m_widget->GetTween()->Play();
}



/**
 * @brief トゥイーンアニメーションのリセット
 *
 * @param なし
 *
 * @return なし
 */
void InputGuideUI::Reset()
{
	m_widget->TweenReset();
}
