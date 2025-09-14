/**
 * @file   StageResultUI.cpp
 *
 * @brief  StageResultUIに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "StageResultUI.h"

using namespace DirectX;


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
void StageResultUI::Initialize(ID3D11ShaderResourceView* texture,
	DirectX::SimpleMath::Vector2 texSize,
	DirectX::SimpleMath::Vector2 windowSize)
{
	SimpleMath::Vector2 startPos = SimpleMath::Vector2
	{
		windowSize.x * 0.5f,
		texSize.y * 0.5f
	};

	m_widget = std::make_unique<UIWidget>();
	Tween::TweenData data =
	{
		Tween::UIParams{startPos,SimpleMath::Vector2(1.0f,1.0f),0.0f,1.0f},
		Tween::UIParams{SimpleMath::Vector2(0.0f, texSize.y * 0.5f),SimpleMath::Vector2(0.0f,0.0f),0.0f,0.0f},
		0.5f,
		Tween::Ease::OutQuart,
		Tween::PlaybackMode::Once
	};
	m_widget->Initialize(texture, data, texSize, false);
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

	if (m_widget->GetTween()->Finished()) m_widget->TweenReset(false);
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

