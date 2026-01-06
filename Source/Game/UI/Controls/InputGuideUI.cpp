/**
 * @file   InputGuideUI.cpp
 *
 * @brief  操作ガイドUIに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "InputGuideUI.h"
#include "Source/Game/UI/Elements/Atlas/KeyAtlas.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
InputGuideUI::InputGuideUI()
	: UIElement()
	, m_pKbTracker(nullptr)
	, m_pressed(false)
	, m_textTexture(nullptr)
{

}



/**
 * @brief デストラクタ
 */
InputGuideUI::~InputGuideUI()
{

}



/**
 * @brief 初期化処理
 *
 * @param textTexture	文字テクスチャのポインタ
 * @param baseTexture	背景テクスチャのポインタ
 * @param pos			表示位置
 * @param size			テクスチャのサイズ
 * @param key			対応キー
 * @param pKbTracker	キーボードトラッカーのポインタ
 *
 * @return なし
 */
void InputGuideUI::Initialize(
	ID3D11ShaderResourceView* textTexture,
	ID3D11ShaderResourceView* baseTexture,
	DirectX::SimpleMath::Vector2 pos,
	DirectX::SimpleMath::Vector2 size,
	int textWidth,
	std::vector<DirectX::Keyboard::Keys> keys,
	DirectX::Keyboard::KeyboardStateTracker* pKbTracker)
{
	// トゥイーンの設定
	Tween2D::TweenData data =
	{
		Tween2D::UIParams{pos,DirectX::SimpleMath::Vector2(0.5f), 0.0f, 1.0f},
		Tween2D::UIParams{DirectX::SimpleMath::Vector2(0.0f),
						  DirectX::SimpleMath::Vector2(-0.1f),
						  0.0f, -0.2f},
		0.2f,
		Easing::EaseType::OutQuart,
		Easing::PlaybackMode::Once
	};
	// ウィジェットの作成
	m_widget = std::make_unique<UIWidget>();
	m_widget->Initialize(baseTexture, data, size, false);

	// 対応キーを登録
	m_inputKeys = keys;

	// キーボードトラッカーのポインタの登録
	m_pKbTracker = pKbTracker;

	// 押下状態の初期化
	m_pressed = false;

	// 文字テクスチャを登録
	m_textTexture = textTexture;

	// テキスト画像の1文字分の幅を設定
	m_textWidth = textWidth;
	
	// キー押下状態記録用配列を作成
	for (int i = 0; i < m_inputKeys.size(); i++)
	{
		m_keyLastStates.push_back(false);
	}
}



/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void InputGuideUI::Update(float elapsedTime)
{	
	// 各キーの押下状態をチェック
	for (int i = 0; i < m_inputKeys.size(); i++)
	{
		if (m_pKbTracker->IsKeyPressed(m_inputKeys[i]))
		{
			m_keyLastStates[i] = true;
		}
		else if (m_pKbTracker->IsKeyReleased(m_inputKeys[i]))
		{
			m_keyLastStates[i] = false;
		}
	}

	// 全体のキー押下状態を更新
	int pressNum = 0;
	for (bool keyState : m_keyLastStates)
	{
		if (keyState) pressNum++;
	}
	if (pressNum == 0) m_pressed = false;
	else			   m_pressed = true;


	// キー状態に応じて再生/停止
	if (m_pressed && !m_widget->GetTween()->IsPlayed())
	{
		m_widget->GetTween()->Play();
	}
	else if (!m_pressed)
	{
		m_widget->TweenReset(false);
	}

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
void InputGuideUI::Draw(RenderContext context)
{
	RECT rect;
	DirectX::SimpleMath::Vector2 pos = DirectX::SimpleMath::Vector2::Zero;

	for (DirectX::Keyboard::Keys key : m_inputKeys)
	{
		// 背景の描画
		m_widget->Draw(context.spriteBatch, nullptr, pos);

		// 文字テクスチャの切り取り範囲を計算
		RECT rect = KeyAtlas::GetRect(key, m_textWidth);

		// 文字の描画
		m_widget->Draw(context.spriteBatch, m_textTexture, pos, &rect);

		// 表示位置をずらす
		pos.x += m_widget->GetTexSize().x;
	}
}



/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void InputGuideUI::Finalize()
{
	m_textTexture = nullptr;

	if (m_widget) m_widget->Finalize();
	m_widget.reset();
}