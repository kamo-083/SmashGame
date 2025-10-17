/**
 * @file   Button.cpp
 *
 * @brief  Buttonに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Button.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
Button::Button()
{

}



/**
 * @brief デストラクタ
 */
Button::~Button()
{

}



/**
 * @brief 初期化処理
 *
 * @param なし
 *
 * @return なし
 */
void Button::Initialize(ID3D11ShaderResourceView* texture,
						const Tween::TweenData data,
						DirectX::SimpleMath::Vector2 size,
						std::function<void()> operate)
{
	m_widget = std::make_unique<UIWidget>();
	m_widget->Initialize(texture, data, size);

	m_operate = operate;
}



/**
 * @brief 更新処理
 *
 * @param なし
 *
 * @return なし
 */
void Button::Update(float elapsedTime)
{
	m_widget->Update(elapsedTime);
}



/**
 * @brief 描画処理
 *
 * @param なし
 *
 * @return なし
 */
void Button::Draw(RenderContext context)
{
	m_widget->Draw(context);
}



/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void Button::Finalize()
{
	if(m_widget) m_widget->Finalize();
	m_widget.reset();

	m_operate = nullptr;
}


void Button::Press()
{
	if (m_operate)	m_operate();
}

void Button::Reset()
{
	m_widget->TweenReset();
}
