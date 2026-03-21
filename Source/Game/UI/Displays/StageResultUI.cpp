/**
 * @file   StageResultUI.cpp
 *
 * @brief  リザルトUIに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "StageResultUI.h"
#include "Source/Game/UI/Elements/NumberRenderer/NumberRenderer2D.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
StageResultUI::StageResultUI()
	:
	UIElement(),
	m_enable(false),
	m_clearTime{}
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
 * @param texture		テクスチャの情報
 * @param windowSize	ウィンドウのサイズ
 *
 * @return なし
 */
void StageResultUI::Initialize(
	const Textures& textures,
	const DirectX::SimpleMath::Vector2& windowSize)
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
		ANIM_TIME,
		Easing::EaseType::OutBack,
		Easing::PlaybackMode::Once
	};
	m_widget->Initialize(textures.result.texture, data, textures.result.size);

	// 数字描画機能
	m_number = std::make_unique<NumberRenderer2D>(
		INumberRenderer::DisplayMode::Default, textures.number.size, textures.number.texture, TIME_DIGIT);

	// クリアタイムのテキスト画像のポインタを初期化
	m_clearTimeTexture = textures.clearTime.texture;

	// クリアタイムの表示位置を初期化
	m_clearTimePosition = m_widget->GetParam().pos;
	m_clearTimePosition.x -= textures.clearTime.size.x * 0.5f;
	m_clearTimePosition.y -= textures.number.size.y * 0.5f;

	// 無効化
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
void StageResultUI::Draw(const RenderContext& context)
{
	if (!m_enable) return;

	// ウィジェットの描画
	m_widget->Draw(context.spriteBatch);

	// クリアタイムの描画
	// テキスト
	context.spriteBatch->Draw(m_clearTimeTexture.Get(), m_clearTimePosition);

	// 数字
	// 描画位置を初期化
	DirectX::SimpleMath::Vector2 numberPos = m_widget->GetParam().pos;
	numberPos.x += m_number->GetSpriteSize().x * 0.5f;
	numberPos.y += m_number->GetSpriteSize().y;

	// 表示間隔を設定 (分と秒の間隔)
	float numberWidth = m_number->GetSpriteSize().x * NUMBER_WIDTH_SCALE;

	// 分
	// 描画位置を設定
	m_number->SetPosition(DirectX::SimpleMath::Vector2(numberPos.x - numberWidth, numberPos.y));
	// 数字を設定
	m_number->SetNumber(m_clearTime.minute);
	// 描画
	m_number->Draw(context);

	// 秒
	// 描画位置を設定
	m_number->SetPosition(DirectX::SimpleMath::Vector2(numberPos.x + numberWidth, numberPos.y));
	// 数字を設定
	m_number->SetNumber(m_clearTime.second);
	// 描画
	m_number->Draw(context);
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



/**
 * @brief クリアタイムの設定
 *
 * @param time 経過時間(秒)
 *
 * @return なし
 */
void StageResultUI::SetClearTime(float time)
{
	int totalSeconds = static_cast<int>(time);

	m_clearTime.minute = totalSeconds / 60;	// 分
	m_clearTime.second = totalSeconds % 60;	// 秒
}

