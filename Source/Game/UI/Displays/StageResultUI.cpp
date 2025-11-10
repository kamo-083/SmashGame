/**
 * @file   StageResultUI.cpp
 *
 * @brief  リザルトUIに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "StageResultUI.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
StageResultUI::StageResultUI()
	: UIElement()
	, m_enable(false)
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
 * @param texture		テクスチャのポインタ
 * @param texSize		テクスチャのサイズ
 * @param windowSize	ウィンドウのサイズ
 *
 * @return なし
 */
void StageResultUI::Initialize(
	ID3D11ShaderResourceView* texture,
	DirectX::SimpleMath::Vector2 texSize,
	DirectX::SimpleMath::Vector2 windowSize)
{
	// 表示位置の計算
	DirectX::SimpleMath::Vector2 pos = DirectX::SimpleMath::Vector2
	{
		windowSize.x * 0.5f,
		windowSize.y * 0.5f
	};

	// ウィジェットの作成
	m_widget = std::make_unique<UIWidget>();
	// トゥイーンパラメータの作成
	Tween2D::TweenData data =
	{
		Tween2D::UIParams{pos,DirectX::SimpleMath::Vector2(1.0f,0.0f),0.0f,1.0f},
		Tween2D::UIParams{DirectX::SimpleMath::Vector2(0.0f, 0.0f),DirectX::SimpleMath::Vector2(0.0f,1.0f),0.0f,0.0f},
		0.25f,
		Tween2D::Ease::OutBack,
		Tween2D::PlaybackMode::Once
	};
	m_widget->Initialize(texture, data, texSize);

	m_enable = false;
}



/**
 * @brief 更新処理
 *
 * @param elapsedTime	経過時間
 *
 * @return なし
 */
void StageResultUI::Update(float elapsedTime)
{
	if (!m_enable) return;

	// ウィジェットの更新
	m_widget->Update(elapsedTime);
}



/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 *
 * @return なし
 */
void StageResultUI::Draw(RenderContext context)
{
	if (!m_enable) return;

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
void StageResultUI::Finalize()
{
	if (m_widget) m_widget->Finalize();
	m_widget.reset();
}

