/**
 * @file   InputGuideUI.cpp
 *
 * @brief  InputGuideUIに関するソースファイル
 *
 * @author 清水まこと
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "InputGuideUI.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
InputGuideUI::InputGuideUI()
{

}



/**
 * @brief デストラクタ
 */
InputGuideUI::~InputGuideUI()
{

}



/**
 * @brief 操作ガイドの追加
 *
 * @param[in] なし
 *
 * @return なし
 */
void InputGuideUI::AddGuide(ID3D11ShaderResourceView* texture, const Tween::TweenData data, DirectX::SimpleMath::Vector2 size)
{
	// 幅とかはこのクラスで持っておいて、大きさとか画像とかの基本的な情報を貰ってこっち側でTween作れるようにする

	std::unique_ptr<Button> guide = std::make_unique<Button>();
	guide->Initialize(texture, data, size);
	m_guides.push_back(std::move(guide));
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void InputGuideUI::Update(float elapsedTime)
{
	for (auto& guide : m_guides)
	{
		guide->Update(elapsedTime);
	}
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void InputGuideUI::Draw(RenderContext context)
{
	for (auto& guide : m_guides)
	{
		guide->Draw(context);
	}
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void InputGuideUI::Finalize()
{
	for (auto& guide : m_guides)
	{
		guide->Finalize();
	}
	m_guides.clear();
}
