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
	: 
	UIElement(),
	CONDITIONS_TYPE{ clearCondition },
	m_isIntroPlayed(false)
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
 * @param textureInfo	テクスチャ情報
 *
 * @return なし
 */
void ClearConditionsUI::Initialize(
	const DirectX::SimpleMath::Vector2& windowSize,
	const TextureInfo& textureInfo)
{
	// 別名
	using Vector2 = DirectX::SimpleMath::Vector2;

	m_textSize = textureInfo.size;

	// 開始直後のアニメーションのパラメータ
	m_introParam =
	{
		Vector2(-m_textSize.x,	windowSize.y * 0.25f),		// 開始位置
		Vector2(windowSize.x + m_textSize.x * 2.0f, 0.0f),	// 変化値
	};

	// 画面端へのアニメーションのパラメータ
	Vector2 cornerSize = m_textSize * TEXT_SIZE_CORNER;
	m_cornerParam =
	{
		Vector2(-cornerSize.x, cornerSize.y * 0.5f),				// 開始位置
		Vector2(cornerSize.x * 1.5f + TEXT_MARGIN_CORNER, 0.0f),	// 変化値
	};

	// ウィジェットの作成
	m_widget = std::make_unique<UIWidget>();
	// トゥイーンパラメータの作成
	Tween2D::TweenData data =
	{
		Tween2D::UIParams{m_introParam.start, DirectX::SimpleMath::Vector2::One, 0.0f, 1.0f},
		Tween2D::UIParams{m_introParam.delta, DirectX::SimpleMath::Vector2::Zero, 0.0f, 0.0f},
		ANIM_TIME,
		Easing::EaseType::OutInQuart,
		Easing::PlaybackMode::Once
	};
	m_widget->Initialize(textureInfo.texture, data, m_textSize);
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

	// 開始直後のアニメーションが終わっていたら
	if (!m_isIntroPlayed && m_widget->GetTween()->Finished())
	{
		// 画面端へのパラメータを設定
		m_widget->SetParam(
			Tween2D::UIParams{ m_cornerParam.start, DirectX::SimpleMath::Vector2(TEXT_SIZE_CORNER), 0.0f, 1.0f },
			Tween2D::UIParams{ m_cornerParam.delta, DirectX::SimpleMath::Vector2::Zero, 0.0f, 0.0f }
		);
		// イージングの種類を設定
		m_widget->SetEaseType(Easing::EaseType::OutQuart);

		// アニメーションをリセット
		m_widget->TweenReset();

		m_isIntroPlayed = true;
	}
}



/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 *
 * @return なし
 */
void ClearConditionsUI::Draw(const RenderContext& context)
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
	m_widget->Draw(
		context.spriteBatch,
		nullptr,
		DirectX::SimpleMath::Vector2::Zero,
		&rect
	);
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

