/**
 * @file   SlidePanel.cpp
 *
 * @brief  SlidePanelに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "SlidePanel.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
SlidePanel::SlidePanel(int stage_num)
	: STAGE_NUM{ stage_num }
	, m_lastDirection{ DIRECTION::RIGHT }
{

}



/**
 * @brief デストラクタ
 */
SlidePanel::~SlidePanel()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void SlidePanel::Initialize(ID3D11ShaderResourceView* texture,
							DirectX::SimpleMath::Vector2 texSize,
							DirectX::SimpleMath::Vector2 windowSize)
{
	SimpleMath::Vector2 startPos =
		SimpleMath::Vector2(texSize.x * 0.75f + windowSize.x / 3.0f * STAGE_NUM, windowSize.y * 0.5f);

	m_UI = std::make_unique<UIWidget>();
	Tween::TweenData data =
	{
		Tween::UIParams{startPos,SimpleMath::Vector2(1.0f,1.0f),0.0f,1.0f},
		Tween::UIParams{SimpleMath::Vector2(texSize.x, 0.0f),SimpleMath::Vector2(0.0f,0.0f),0.0f,0.0f},
		0.5f,
		Tween::Ease::OutQuart,
		Tween::PlaybackMode::Once
	};
	m_UI->Initialize(texture, data, texSize, false);
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void SlidePanel::Update(float elapsedTime)
{
	m_UI->Update(elapsedTime);

	if (m_UI->GetTween()->Finished()) m_UI->TweenReset(false);
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void SlidePanel::Draw(RenderContext context)
{
	m_UI->Draw(context);
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void SlidePanel::Finalize()
{
	if (m_UI) m_UI->Finalize();
	m_UI.reset();
}

void SlidePanel::Slide(DIRECTION dir)
{
	if (dir == DIRECTION::NONE) return;

	if (dir != m_lastDirection)
	{
		m_UI->GetTween()->ReverseDeltaParam();
		m_lastDirection = dir;
	}

	m_UI->TweenReset(true);
}
