/**
 * @file   PauseUI.cpp
 *
 * @brief  ポーズUIに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "PauseUI.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
PauseUI::PauseUI()
	: UIElement()
	, m_isOpen(false)
	, m_options(PAUSE_OPTIONS::NONE)
{

}



/**
 * @brief デストラクタ
 */
PauseUI::~PauseUI()
{

}



/**
 * @brief 初期化処理
 *
 * @param windowSize	ウィンドウサイズ
 * @param textures		テクスチャ群のポインタ
 *
 * @return なし
 */
void PauseUI::Initialize(
	const DirectX::SimpleMath::Vector2& windowSize,
	Textures& textures)
{
	// 開かれているかのフラグを初期化
	m_isOpen = false;

	// 選択項目の初期化
	m_options = PAUSE_OPTIONS::RETURN_STAGE;

	// ウィンドウサイズの設定
	m_windowSize = windowSize;

	// テクスチャ群の設定
	m_textures = std::make_unique<Textures>();
	m_textures->window = textures.window;
	m_textures->text = textures.text;

	// 背景
	m_widget = std::make_unique<UIWidget>();
	Tween2D::UIParams start{
		m_windowSize * 0.5f,
		DirectX::SimpleMath::Vector2(1.0f,0.0f),
		0.0f,1.0f
	};
	Tween2D::UIParams delta{
		DirectX::SimpleMath::Vector2::Zero,
		DirectX::SimpleMath::Vector2(0.0f,1.0f),
		0.0f,1.0f
	};
	Tween2D::TweenData params{
		start,delta,
		0.15f,
		Easing::EaseType::InSine,
		Easing::PlaybackMode::Once
	};
	m_widget->Initialize(m_textures->window, params, DirectX::SimpleMath::Vector2(350.f, 267.f), false);
}



/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void PauseUI::Update(float elapsedTime)
{
	if (!m_isOpen) return;	// 開いていなかったら終了

	// ウィジェットの更新
	m_widget->Update(elapsedTime);
}



/**
 * @brief 描画処理
 *
 * @param context 描画用構造体
 *
 * @return なし
 */
void PauseUI::Draw(RenderContext context)
{
	if (!m_isOpen) return;	// 開いていなかったら終了

	// ウィジェットの描画
	m_widget->Draw(context.spriteBatch);

	// テキスト描画の初期設定
	float height = 60;
	float numHalf = std::floor(static_cast<int>(PAUSE_OPTIONS::OPTIONS_NUM) / 2);	// 項目をずらす数を求める
	DirectX::SimpleMath::Vector2 textPos = { 0,-(height * numHalf) };				// 真ん中の項目がウィンドウの中央に来るようにずらす
	RECT rect = { 0,0,0,0 };
	rect.right = 350;
	rect.bottom = height;
	DirectX::SimpleMath::Color color = { 1,1,1,0 };

	// 項目数だけ回す
	for (int i = 0; i < static_cast<int>(PAUSE_OPTIONS::OPTIONS_NUM); i++)
	{
		// 選択されている項目だったら色を変える
		if (i == static_cast<int>(m_options))	color.z = 0;	// 黄
		else									color.z = 1;	// 白

		// テキストを描画
		m_widget->Draw(
			context.spriteBatch,
			m_textures->text,
			textPos, &rect, FLT_MAX, color);

		// 表示位置をずらす
		textPos.y += height;
		rect.top += height;
		rect.bottom += height;
	}
}



/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void PauseUI::Finalize()
{
	if (m_widget) m_widget->Finalize();
	m_widget.reset();

	m_textures.reset();
}



/**
 * @brief ポーズを開く
 *
 * @param なし
 *
 * @return なし
 */
void PauseUI::OpenPause()
{
	// アニメーションを再生
	m_widget->GetTween()->Play();

	// フラグを開いている状態に設定
	m_isOpen = true;
}



/**
 * @brief ポーズを閉じる
 *
 * @param なし
 *
 * @return なし
 */
void PauseUI::ClosePause()
{
	// アニメーションを初期化
	m_widget->TweenReset();

	// フラグを閉じている状態に設定
	m_isOpen = false;

	// 選択項目を初期化
	m_options = PAUSE_OPTIONS::RETURN_STAGE;
}



/**
 * @brief 選択項目を上に動かす
 *
 * @param なし
 *
 * @return なし
 */
void PauseUI::SelectUp()
{
	m_options = static_cast<PAUSE_OPTIONS>(static_cast<int>(m_options) - 1);

	// 選択範囲を超えていた場合はループする
	if (m_options == PAUSE_OPTIONS::NONE)
	{
		m_options = static_cast<PAUSE_OPTIONS>(static_cast<int>(PAUSE_OPTIONS::OPTIONS_NUM) - 1);
	}
}



/**
 * @brief 選択項目を下に動かす
 *
 * @param なし
 *
 * @return なし
 */
void PauseUI::SelectDown()
{
	m_options = static_cast<PAUSE_OPTIONS>(static_cast<int>(m_options) + 1);

	// 選択範囲を超えていた場合はループする
	if (m_options == PAUSE_OPTIONS::OPTIONS_NUM)
	{
		m_options = static_cast<PAUSE_OPTIONS>(static_cast<int>(PAUSE_OPTIONS::NONE) + 1);
	}
}
