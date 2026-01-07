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
 * @param textures		テクスチャ情報
 *
 * @return なし
 */
void PauseUI::Initialize(
	const DirectX::SimpleMath::Vector2& windowSize,
	const Textures& textures)
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
	m_textures->titleText = textures.titleText;
	m_textures->optionsText = textures.optionsText;

	// ウィジェットの作成
	SetupWidget();
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
	m_widget->Update(elapsedTime);			// 背景
	m_titleWidget->Update(elapsedTime);		// タイトル
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
	m_widget->Draw(context.spriteBatch);		// 背景
	m_titleWidget->Draw(context.spriteBatch);	// タイトル

	// 選択肢テキストの初期設定
	float numHalf = std::floorf(static_cast<float>(PAUSE_OPTIONS::OPTIONS_NUM) / 2.0f);		// 項目をずらす数を求める
	DirectX::SimpleMath::Vector2 textPos = 
	{ 0,-(static_cast<LONG>(m_textures->optionsText.size.y) * numHalf) };	// 真ん中の項目がウィンドウの中央に来るように位置をずらす
	RECT rect = { 0,0,0,0 };
	rect.right = static_cast<LONG>(m_textures->optionsText.size.x);
	rect.bottom = static_cast<LONG>(m_textures->optionsText.size.y);
	DirectX::SimpleMath::Color color = { 1,1,1,0 };

	// 項目数だけ回す
	for (int i = 0; i < static_cast<int>(PAUSE_OPTIONS::OPTIONS_NUM); i++)
	{
		// 選択されている項目だったら色を変える
		if (i == static_cast<int>(m_options))	color = SELECTED_COLOR;
		else									color = UNSELECTED_COLOR;

		// 選択肢テキストを描画
		m_widget->Draw(
			context.spriteBatch,
			m_textures->optionsText.texture.Get(),
			textPos, &rect, FLT_MAX, color);

		// 表示位置をずらす
		textPos.y += static_cast<LONG>(m_textures->optionsText.size.y);
		rect.top += static_cast<LONG>(m_textures->optionsText.size.y);
		rect.bottom += static_cast<LONG>(m_textures->optionsText.size.y);
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
	
	if (m_titleWidget) m_titleWidget->Finalize();
	m_titleWidget.reset();

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
	m_widget->GetTween()->Play();		// 背景
	m_titleWidget->GetTween()->Play();	// タイトル

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
	m_widget->TweenReset();			// 背景
	m_titleWidget->TweenReset();	// タイトル

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



/**
 * @brief ウィジェットの初期設定
 *
 * @param なし
 *
 * @return なし
 */
void PauseUI::SetupWidget()
{
	// 背景
	m_widget = std::make_unique<UIWidget>();
	Tween2D::UIParams start_back{
		m_windowSize * 0.5f,
		DirectX::SimpleMath::Vector2(1.0f, 0.0f),
		0.0f,1.0f
	};
	Tween2D::UIParams delta_back{
		DirectX::SimpleMath::Vector2::Zero,
		DirectX::SimpleMath::Vector2(0.0f, 1.0f),
		0.0f,0.0f
	};
	Tween2D::TweenData params_back{
		start_back,delta_back,
		WINDOW_ANIM_TIME,
		Easing::EaseType::OutBack,
		Easing::PlaybackMode::Once
	};
	m_widget->Initialize(m_textures->window.texture, params_back, m_textures->window.size, false);

	// タイトル
	m_titleWidget = std::make_unique<UIWidget>();
	Tween2D::UIParams start_title{
		DirectX::SimpleMath::Vector2(start_back.pos.x, start_back.pos.y - TITLE_POS_ADJUST),
		DirectX::SimpleMath::Vector2(1.0f, 1.0f),
		0.0f,1.0f
	};
	Tween2D::UIParams delta_title{
		DirectX::SimpleMath::Vector2(0.0f, TEXT_TITLE_MOVE),
		DirectX::SimpleMath::Vector2(0.0f, 0.0f),
		0.0f,0.0f
	};
	Tween2D::TweenData params_title{
		start_title,delta_title,
		TITLE_ANIM_TIME,
		Easing::EaseType::InOutSine,
		Easing::PlaybackMode::PingPong
	};
	m_titleWidget->Initialize(m_textures->titleText.texture, params_title, m_textures->titleText.size, false);
}
