/**
 * @file   ClearConditionsUI.cpp
 *
 * @brief  ClearConditionsUIに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "ClearConditionsUI.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
ClearConditionsUI::ClearConditionsUI(ConditionsType clearCondition)
	:CONDITIONS_TYPE{ clearCondition }
{

}



/**
 * @brief デストラクタ
 */
ClearConditionsUI::~ClearConditionsUI()
{

}



/**
 * @brief 初期化処理
 *
 * @param なし
 *
 * @return なし
 */
void ClearConditionsUI::Initialize(
	DirectX::SimpleMath::Vector2 windowSize,
	ResourceManager* pRM)
{
	DirectX::SimpleMath::Vector2 startPos = DirectX::SimpleMath::Vector2
	{
		-TEXT_SIZE.x,
		windowSize.y * 0.25f
	};
	DirectX::SimpleMath::Vector2 endPos = DirectX::SimpleMath::Vector2
	{
		windowSize.x + TEXT_SIZE.x * 2.0f,
		0.0f
	};

	m_widget = std::make_unique<UIWidget>();
	Tween::TweenData data =
	{
		Tween::UIParams{startPos,DirectX::SimpleMath::Vector2::One,0.0f,1.0f},
		Tween::UIParams{endPos,DirectX::SimpleMath::Vector2::Zero,0.0f,0.0f},
		2.0f,
		Tween::Ease::OutInQuart,
		Tween::PlaybackMode::Once
	};
	m_widget->Initialize(
		pRM->RequestPNG("conditionsText", L"Resources/Textures/Text/conditionsText.png"),
		data, TEXT_SIZE);
}



/**
 * @brief 更新処理
 *
 * @param なし
 *
 * @return なし
 */
void ClearConditionsUI::Update(float elapsedTime)
{
	m_widget->Update(elapsedTime);
}



/**
 * @brief 描画処理
 *
 * @param なし
 *
 * @return なし
 */
void ClearConditionsUI::Draw(RenderContext context)
{
	// 画像の切り取り範囲を設定
	float height = TEXT_SIZE.y * static_cast<float>(CONDITIONS_TYPE);
	RECT rect =
	{
		0.0f,
		height,
		TEXT_SIZE.x,
		height + TEXT_SIZE.y,
	};

	// 画像の描画
	context.spriteBatch->Begin(
		DirectX::SpriteSortMode_Deferred,
		context.states->NonPremultiplied(),
		context.states->LinearClamp());
	m_widget->Draw(
		context.spriteBatch,
		nullptr,
		DirectX::SimpleMath::Vector2::Zero,
		&rect
		);
	context.spriteBatch->End();
}



/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void ClearConditionsUI::Finalize()
{
	if (m_widget) m_widget->Finalize();
	m_widget.reset();
}

