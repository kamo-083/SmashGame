/**
 * @file   ClearConditionsUI.cpp
 *
 * @brief  クリア条件UIに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "ClearConditionsUI.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param clearCondition	クリア条件
 */
ClearConditionsUI::ClearConditionsUI(ConditionsType clearCondition)
	: UIElement()
	, CONDITIONS_TYPE{ clearCondition }
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
 * @param windowSize	ウィンドウサイズ
 * @param textTex		テキストの画像
 * @param textSize		テキストの画像の大きさ(1行分)
 *
 * @return なし
 */
void ClearConditionsUI::Initialize(
	DirectX::SimpleMath::Vector2 windowSize,
	ID3D11ShaderResourceView* textTex,
	DirectX::SimpleMath::Vector2 textSize)
{
	m_textSize = textSize;

	// 開始位置の計算
	DirectX::SimpleMath::Vector2 startPos = DirectX::SimpleMath::Vector2
	{
		-m_textSize.x,
		windowSize.y * 0.25f
	};
	// 終了位置の計算
	DirectX::SimpleMath::Vector2 endPos = DirectX::SimpleMath::Vector2
	{
		windowSize.x + m_textSize.x * 2.0f,
		0.0f
	};

	// ウィジェットの作成
	m_widget = std::make_unique<UIWidget>();
	// トゥイーンパラメータの作成
	Tween2D::TweenData data =
	{
		Tween2D::UIParams{startPos,DirectX::SimpleMath::Vector2::One,0.0f,1.0f},
		Tween2D::UIParams{endPos,DirectX::SimpleMath::Vector2::Zero,0.0f,0.0f},
		TWEEN_ANIM_TIME,
		Easing::EaseType::OutInQuart,
		Easing::PlaybackMode::Once
	};
	m_widget->Initialize(textTex, data, m_textSize);		
}



/**
 * @brief 更新処理
 *
 * @param elapsedTime	経過時間
 *
 * @return なし
 */
void ClearConditionsUI::Update(float elapsedTime)
{
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
void ClearConditionsUI::Draw(RenderContext context)
{
	// 画像の切り取り範囲を設定
	LONG height = static_cast<LONG>(m_textSize.y) * static_cast<LONG>(CONDITIONS_TYPE);
	RECT rect =
	{
		0,
		height,
		static_cast<LONG>(m_textSize.x),
		height + static_cast<LONG>(m_textSize.y),
	};

	// 画像の描画
	//context.spriteBatch->Begin(
	//	DirectX::SpriteSortMode_Deferred,
	//	context.states->NonPremultiplied(),
	//	context.states->LinearClamp());
	m_widget->Draw(
		context.spriteBatch,
		nullptr,
		DirectX::SimpleMath::Vector2::Zero,
		&rect
		);
	//context.spriteBatch->End();
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

