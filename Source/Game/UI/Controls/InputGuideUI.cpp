/**
 * @file   InputGuideUI.cpp
 *
 * @brief  操作ガイドUIに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "InputGuideUI.h"
#include "Source/Game/Common/KeyConverter.h"


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
 * @param texture	 テクスチャのポインタ
 * @param pos		 表示位置
 * @param size		 テクスチャのサイズ
 * @param key		 対応キー
 * @param pKbTracker キーボードトラッカーのポインタ
 *
 * @return なし
 */
void InputGuideUI::Initialize(
	ID3D11ShaderResourceView* texture,
	DirectX::SimpleMath::Vector2 pos,
	DirectX::SimpleMath::Vector2 size,
	std::vector<DirectX::Keyboard::Keys> keys,
	DirectX::Keyboard::KeyboardStateTracker* pKbTracker)
{
	Tween2D::TweenData data =
	{
		Tween2D::UIParams{pos,DirectX::SimpleMath::Vector2(1.0f,1.0f), 0.0f, 1.0f},
		Tween2D::UIParams{DirectX::SimpleMath::Vector2(0.0f),
						  DirectX::SimpleMath::Vector2(- 0.2f),
						  0.0f, -0.2f},
		0.5f,
		Tween2D::Ease::OutQuart,
		Tween2D::PlaybackMode::Once
	};
	// ウィジェットの作成
	m_widget = std::make_unique<UIWidget>();
	m_widget->Initialize(texture, data, size, false);

	// 対応キーを登録
	m_keys = keys;

	// キーボードトラッカーのポインタの登録
	m_pKbTracker = pKbTracker;

	// 押下状態の初期化
	m_pressed = false;

	for (int i = 0; i < m_keys.size(); i++)
	{
		m_keyLastStates.push_back(false);
	}
}



/**
 * @brief 更新処理
 *
 * @param なし
 *
 * @return なし
 */
void InputGuideUI::Update(float elapsedTime)
{	
	// 複数キー対応
	for (int i = 0; i < m_keys.size(); i++)
	{
		if (m_pKbTracker->IsKeyPressed(m_keys[i]))
		{
			m_keyLastStates[i] = true;
		}
		else if (m_pKbTracker->IsKeyReleased(m_keys[i]))
		{
			m_keyLastStates[i] = false;
		}
	}

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
 * @param なし
 *
 * @return なし
 */
void InputGuideUI::Draw(RenderContext context)
{
	long size = 120;
	int keyIndex = KeyConverter::GetIndex(m_keys[0]);
	RECT rect;
	if (keyIndex >= 30)
	{
		// 特殊キー(4列目以降)
		long height = size * (keyIndex - 30 + 3);
		rect =
		{
			0,	  height,
			size, height + size
		};
	}
	else if (keyIndex >= 26)
	{
		// 矢印キー(3列目)
		long height = size * 2;
		long width = size * (keyIndex - 26);
		rect =
		{
			width,		  height,
			width + size, height + size
		};
	}
	else if (keyIndex >= 13)
	{
		// アルファベット2列目
		long width = size * (keyIndex - 13);
		rect =
		{
			width,		  size,
			width + size, size * 2
		};
	}
	else
	{
		// アルファベット1列目
		long width = size * keyIndex;
		rect =
		{
			width,		  0,
			width + size, size
		};
	}

	// ウィジェットの描画
	m_widget->Draw(context.spriteBatch, nullptr, DirectX::SimpleMath::Vector2::Zero, &rect);
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
	if (m_widget) m_widget->Finalize();
	m_widget.reset();
}