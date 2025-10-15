/**
 * @file   StageResultUI.cpp
 *
 * @brief  StageResultUIに関するソースファイル
 *
 * @author 清水まこと
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "StageResultUI.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
StageResultUI::StageResultUI()
{

}



/**
 * @brief デストラクタ
 */
StageResultUI::~StageResultUI()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void StageResultUI::Initialize(
	ID3D11ShaderResourceView* texture,
	DirectX::SimpleMath::Vector2 texSize,
	DirectX::SimpleMath::Vector2 windowSize)
{
	DirectX::SimpleMath::Vector2 startPos = DirectX::SimpleMath::Vector2
	{
		windowSize.x * 0.5f,
		windowSize.y * 0.5f
	};

	m_widget = std::make_unique<UIWidget>();
	Tween::TweenData data =
	{
		Tween::UIParams{startPos,DirectX::SimpleMath::Vector2(1.0f,0.0f),0.0f,1.0f},
		Tween::UIParams{DirectX::SimpleMath::Vector2(0.0f, 0.0f),DirectX::SimpleMath::Vector2(0.0f,1.0f),0.0f,0.0f},
		0.25f,
		Tween::Ease::OutBack,
		Tween::PlaybackMode::Once
	};
	m_widget->Initialize(texture, data, texSize);
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void StageResultUI::Update(float elapsedTime)
{
	m_widget->Update(elapsedTime);
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void StageResultUI::Draw(RenderContext context)
{
	m_widget->Draw(context);
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void StageResultUI::Finalize()
{
	if (m_widget) m_widget->Finalize();
	m_widget.reset();
}

