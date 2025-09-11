/**
 * @file   Button.cpp
 *
 * @brief  Buttonに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Button.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
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
 * @param[in] なし
 *
 * @return なし
 */
void Button::Initialize(ID3D11ShaderResourceView* texture,
						const Tween::TweenData data,
						DirectX::SimpleMath::Vector2 size,
						std::function<void()> operate)
{
	m_UI = std::make_unique<UIWidget>();
	m_UI->Initialize(texture, data, size);

	m_operate = operate;
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Button::Update(float elapsedTime)
{
	m_UI->Update(elapsedTime);
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Button::Draw(RenderContext context)
{
	m_UI->Draw(context);
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Button::Finalize()
{
	if(m_UI) m_UI->Finalize();
	m_UI.reset();

	m_operate = nullptr;
}


void Button::Press()
{
	m_operate();
}

void Button::Reset()
{
	m_UI->TweenReset();
}
