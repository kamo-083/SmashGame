/**
 * @file   InputHintUI.cpp
 *
 * @brief  操作方法UIに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "InputHintUI.h"
#include "Source/Game/UI/Elements/Atlas/KeyAtlas.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
InputHintUI::InputHintUI()
	: 
	m_textSize(0),
	m_scale(1.0f),
	m_actionTextRect{}
{
}

/**
 * @brief デストラクタ
 */
InputHintUI::~InputHintUI()
{
	m_textures.reset();
}

/**
 * @brief 初期化処理
 *
 * @param なし
 *
 * @return なし
 */
void InputHintUI::Initialize(
	const Textures& textures,
	const DirectX::SimpleMath::Vector2& position,
	float scale,
	long textSize,
	std::vector<DirectX::Keyboard::Keys> keys,
	ActionAtlas::ActionType action)
{
	// 画像を登録
	m_textures = std::make_unique<Textures>();
	m_textures->key = textures.key;
	m_textures->action = textures.action;

	// 表示位置を初期化
	m_position = position;

	// 大きさを初期化
	m_scale = scale;

	// 1文字の大きさを初期化
	m_textSize = textSize;

	// 切り取り範囲を初期化
	// キー
	m_keyTextRect.clear();
	for (DirectX::Keyboard::Keys key : keys)
	{
		m_keyTextRect.push_back(KeyAtlas::GetRect(key, textSize));
	}
	// 動作
	m_actionTextRect = ActionAtlas::GetRect(action, textSize);
}

/**
 * @brief 描画処理
 *
 * @param context 描画用構造体
 *
 * @return なし
 */
void InputHintUI::Draw(const RenderContext& context)
{
	DirectX::SimpleMath::Vector2 pos = m_position;
	DirectX::SimpleMath::Vector2 origin{ m_textSize * 0.5f };

	// テキスト画像を描画
	// キー
	for (RECT rect : m_keyTextRect)
	{
		context.spriteBatch->Draw(
			m_textures->key.Get(), pos, &rect, DirectX::Colors::White, 0.0f, origin, m_scale);

		// 表示位置をずらす
		float width = CalculateWidth(rect);
		pos.x += width;
	}

	// 動作
	context.spriteBatch->Draw(
		m_textures->action.Get(), pos, &m_actionTextRect, DirectX::Colors::White, 0.0f, origin, m_scale);
}

/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void InputHintUI::Finalize()
{
}

/**
 * @brief UI全体の横幅を取得
 *
 * @param なし
 *
 * @return 全体の横幅
 */
float InputHintUI::GetWidth()
{
	float width = 0.0f;

	// キー
	for (RECT rect : m_keyTextRect)
	{
		width += CalculateWidth(rect);
	}

	// 動作
	width += CalculateWidth(m_actionTextRect);

	return width;
}

/**
 * @brief 幅を算出
 *
 * @param rect 計算したい範囲
 *
 * @return 幅
 */
float InputHintUI::CalculateWidth(const RECT rect) const
{
	return (rect.right - rect.left) * m_scale;
}